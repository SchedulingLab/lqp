// SPDX-License-Identifier: MIT
// Copyright (c) 2023-2024 Julien Bernard
#include <lqp/GlpkSolver.h>

#include <cassert>
#include <cstdio>

#include <limits>
#include <memory>

#include <glpk.h>

namespace lqp {
  namespace {
    constexpr double Ignored = 0.0;

    SolutionStatus to_solver_status(int val) {
      switch (val) {
        case GLP_OPT:
          return SolutionStatus::Optimal;
        case GLP_FEAS:
          return SolutionStatus::Feasible;
        case GLP_INFEAS:
          return SolutionStatus::Infeasible;
        case GLP_NOFEAS:
          return SolutionStatus::NoFeasibleSolution;
        case GLP_UNBND:
          return SolutionStatus::UnboundedSolution;
        case GLP_UNDEF:
          return SolutionStatus::Undefined;
        default:
          break;
      }

      return SolutionStatus::Error;
    }

  }

  bool GlpkSolver::available() const {
    return true;
  }

  Solution GlpkSolver::solve(const Problem& problem, const SolverConfig& config) {
    Problem linear_problem = problem;

    if (!linear_problem.is_linear()) {
      auto maybe_linear_problem = problem.linearize();

      if (!maybe_linear_problem) {
        return { SolutionStatus::NotSolved };
      }

      linear_problem = *maybe_linear_problem;
    }

    std::unique_ptr<glp_prob, decltype(&glp_delete_prob)> unique_problem(glp_create_prob(), &glp_delete_prob);
    glp_prob* prob = unique_problem.get();

    std::vector<int> row_indices;
    std::vector<int> col_indices;
    std::vector<double> coefficients;

    // first element is not used by glpk
    row_indices.push_back(0);
    col_indices.push_back(0);
    coefficients.push_back(0.0);

    /*
     * objective
     */

    auto raw_objective = objective(linear_problem);

    glp_set_obj_name(prob, raw_objective.name.c_str());

    switch (raw_objective.sense) {
      case Sense::Maximize:
        glp_set_obj_dir(prob, GLP_MAX);
        break;
      case Sense::Minimize:
        glp_set_obj_dir(prob, GLP_MIN);
        break;
    }

    /*
     * cols (variables)
     */

    auto raw_variables = variables(linear_problem);

    glp_add_cols(prob, static_cast<int>(raw_variables.size()));

    std::size_t col_index = 0;
    int col = 1;

    for (auto & variable : raw_variables) {
      glp_set_col_name(prob, col, variable.name.c_str());

      switch (variable.category) {
        case VariableCategory::Continuous:
          glp_set_col_kind(prob, col, GLP_CV);
          break;
        case VariableCategory::Integer:
          glp_set_col_kind(prob, col, GLP_IV);
          break;
        case VariableCategory::Binary:
          glp_set_col_kind(prob, col, GLP_BV);
          assert(variable.range.type == VariableRange::Bounded);
          break;
      }

      switch (variable.range.type) {
        case VariableRange::Unbounded:
          glp_set_col_bnds(prob, col, GLP_FR, Ignored, Ignored);
          break;
        case VariableRange::LowerBounded:
          glp_set_col_bnds(prob, col, GLP_LO, variable.range.lower, Ignored);
          break;
        case VariableRange::UpperBounded:
          glp_set_col_bnds(prob, col, GLP_UP, Ignored, variable.range.upper);
          break;
        case VariableRange::Bounded:
          if (variable.category != VariableCategory::Binary) {
            glp_set_col_bnds(prob, col, GLP_DB, variable.range.lower, variable.range.upper);
          }
          break;
        case VariableRange::Fixed:
          glp_set_col_bnds(prob, col, GLP_FX, variable.range.lower, variable.range.upper);
          break;
      }

      double coefficient = raw_objective.expression.linear_coefficient(VariableId{col_index});

      if (coefficient != 0.0) {
        glp_set_obj_coef(prob, col, coefficient);
      }

      ++col_index;
      ++col;
    }

    /*
     * rows (constraints)
     */

    auto raw_constraints = constraints(linear_problem);

    glp_add_rows(prob, static_cast<int>(raw_constraints.size()));

    std::size_t row_index = 0;
    int row = 1;

    for (auto & constraint : raw_constraints) {
      glp_set_row_name(prob, row, constraint.name.c_str());

      double constant = constraint.expression.constant();

      switch (constraint.range.type) {
        case VariableRange::Unbounded:
          glp_set_row_bnds(prob, row, GLP_FR, Ignored, Ignored);
          break;
        case VariableRange::LowerBounded:
          glp_set_row_bnds(prob, row, GLP_LO, constraint.range.lower - constant, Ignored);
          break;
        case VariableRange::UpperBounded:
          glp_set_row_bnds(prob, row, GLP_UP, Ignored, constraint.range.upper - constant);
          break;
        case VariableRange::Bounded:
          glp_set_row_bnds(prob, row, GLP_DB, constraint.range.lower - constant, constraint.range.upper - constant);
          break;
        case VariableRange::Fixed:
          glp_set_row_bnds(prob, row, GLP_FX, constraint.range.lower - constant, constraint.range.upper - constant);
          break;
      }

      auto linear_terms = constraint.expression.linear_terms();

      for (auto & term : linear_terms) {
        assert(term.coefficient != 0.0);
        int variable_col = static_cast<int>(to_index(term.variable) + 1);

        row_indices.push_back(row);
        col_indices.push_back(variable_col);
        coefficients.push_back(term.coefficient);
      }

      ++row_index;
      ++row;
    }

    /*
     * matrix
     */

    assert(glp_check_dup(static_cast<int>(raw_constraints.size()), static_cast<int>(raw_variables.size()), static_cast<int>(coefficients.size() - 1), row_indices.data(), col_indices.data()) == 0);
    glp_load_matrix(prob, static_cast<int>(coefficients.size() - 1), row_indices.data(), col_indices.data(), coefficients.data());

    if (!config.problem_output.empty()) {
      glp_write_lp(prob, nullptr, config.problem_output.string().c_str());
    }

    /*
     * solve
     */

    if (config.use_mip) {
      glp_iocp parameters;
      glp_init_iocp(&parameters);

      parameters.msg_lev = config.verbose ? GLP_MSG_ALL : GLP_MSG_OFF;
      parameters.presolve = config.presolve ? GLP_ON : GLP_OFF;
      parameters.tm_lim = (config.timeout != std::chrono::milliseconds::max()) ? static_cast<int>(config.timeout.count()) : std::numeric_limits<int>::max();

      int ret = glp_intopt(prob, &parameters);

      if (!config.solution_output.empty()) {
        glp_print_mip(prob, config.solution_output.string().c_str());
      }

      if (ret == 0) {
        auto status = to_solver_status(glp_mip_status(prob));
        Solution solution(status);

        if (status == SolutionStatus::Optimal || status == SolutionStatus::Feasible) {
          for (std::size_t variable_index = 0; variable_index < raw_variables.size(); ++variable_index) {
            solution.set_value(VariableId{variable_index}, glp_mip_col_val(prob, static_cast<int>(variable_index + 1)));
          }
        }

        return solution;
      }
    } else {
      glp_smcp parameters;
      glp_init_smcp(&parameters);

      parameters.msg_lev = config.verbose ? GLP_MSG_ALL : GLP_MSG_OFF;
      parameters.presolve = config.presolve ? GLP_ON : GLP_OFF;
      parameters.tm_lim = (config.timeout != std::chrono::milliseconds::max()) ? static_cast<int>(config.timeout.count()) : std::numeric_limits<int>::max();

      int ret = glp_simplex(prob, &parameters);

      if (!config.solution_output.empty()) {
        glp_print_sol(prob, config.solution_output.string().c_str());
      }

      if (ret == 0) {
        auto status = to_solver_status(glp_get_status(prob));
        Solution solution(status);

        if (status == SolutionStatus::Optimal || status == SolutionStatus::Feasible) {
          for (std::size_t variable_index = 0; variable_index < raw_variables.size(); ++variable_index) {
            solution.set_value(VariableId{variable_index},  glp_get_col_prim(prob, static_cast<int>(variable_index + 1)));
          }
        }

        return solution;
      }
    }

    return { SolutionStatus::Error };
  }

}


