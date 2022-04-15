#include "GlpkSolver.h"

#include <cassert>
#include <cstdio>
#include <limits>

namespace lqp {
  namespace {
    constexpr double Ignored = 0.0;

    SolverStatus to_solver_status(int val) {
      switch (val) {
        case GLP_OPT:
          return SolverStatus::Optimal;
        case GLP_FEAS:
          return SolverStatus::Feasible;
        case GLP_INFEAS:
          return SolverStatus::Infeasible;
        case GLP_NOFEAS:
          return SolverStatus::NoFeasibleSolution;
        case GLP_UNBND:
          return SolverStatus::UnboundedSolution;
        case GLP_UNDEF:
          return SolverStatus::Undefined;
        default:
          break;
      }

      return SolverStatus::Error;
    }

  }

  GlpkSolver::GlpkSolver()
  : m_problem(nullptr)
  {
  }

  GlpkSolver::~GlpkSolver() {
    if (m_problem != nullptr) {
      glp_delete_prob(m_problem);
    }
  }

  bool GlpkSolver::available() const {
    return true;
  }

  SolverStatus GlpkSolver::solve(const Problem& problem, const SolverConfig& config) {
    Problem linear_problem = problem;

    if (!linear_problem.is_linear()) {
      auto maybe_linear_problem = problem.linearize();

      if (!maybe_linear_problem) {
        return SolverStatus::Error;
      }

      linear_problem = *maybe_linear_problem;
    }

    if (m_problem != nullptr) {
      glp_erase_prob(m_problem);
    } else {
      m_problem = glp_create_prob();
    }

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

    glp_set_obj_name(m_problem, raw_objective.name.c_str());

    switch (raw_objective.sense) {
      case Sense::Maximize:
        glp_set_obj_dir(m_problem, GLP_MAX);
        break;
      case Sense::Minimize:
        glp_set_obj_dir(m_problem, GLP_MIN);
        break;
    }

    /*
     * cols (variables)
     */

    auto raw_variables = variables(linear_problem);

    glp_add_cols(m_problem, raw_variables.size());

    std::size_t col_index = 0;
    int col = 1;

    for (auto & variable : raw_variables) {
      glp_set_col_name(m_problem, col, variable.name.c_str());

      switch (variable.category) {
        case VariableCategory::Continuous:
          glp_set_col_kind(m_problem, col, GLP_CV);
          break;
        case VariableCategory::Integer:
          glp_set_col_kind(m_problem, col, GLP_IV);
          break;
        case VariableCategory::Binary:
          glp_set_col_kind(m_problem, col, GLP_BV);
          assert(variable.range.type == VariableRange::Bounded);
          break;
      }

      switch (variable.range.type) {
        case VariableRange::Unbounded:
          glp_set_col_bnds(m_problem, col, GLP_FR, Ignored, Ignored);
          break;
        case VariableRange::LowerBounded:
          glp_set_col_bnds(m_problem, col, GLP_LO, variable.range.lower, Ignored);
          break;
        case VariableRange::UpperBounded:
          glp_set_col_bnds(m_problem, col, GLP_UP, Ignored, variable.range.upper);
          break;
        case VariableRange::Bounded:
          if (variable.category != VariableCategory::Binary) {
            glp_set_col_bnds(m_problem, col, GLP_DB, variable.range.lower, variable.range.upper);
          }
          break;
        case VariableRange::Fixed:
          glp_set_col_bnds(m_problem, col, GLP_FX, variable.range.lower, variable.range.upper);
          break;
      }

      double coefficient = raw_objective.expression.linear_coefficient(VariableId{col_index});

      if (coefficient != 0.0) {
        glp_set_obj_coef(m_problem, col, coefficient);
      }

      ++col_index;
      ++col;
    }

    /*
     * rows (constraints)
     */

    auto raw_constraints = constraints(linear_problem);

    glp_add_rows(m_problem, raw_constraints.size());

    std::size_t row_index = 0;
    int row = 1;

    for (auto & constraint : raw_constraints) {
      glp_set_row_name(m_problem, row, constraint.name.c_str());

      double constant = constraint.expression.constant();

      switch (constraint.range.type) {
        case VariableRange::Unbounded:
          glp_set_row_bnds(m_problem, row, GLP_FR, Ignored, Ignored);
          break;
        case VariableRange::LowerBounded:
          glp_set_row_bnds(m_problem, row, GLP_LO, constraint.range.lower - constant, Ignored);
          break;
        case VariableRange::UpperBounded:
          glp_set_row_bnds(m_problem, row, GLP_UP, Ignored, constraint.range.upper - constant);
          break;
        case VariableRange::Bounded:
          glp_set_row_bnds(m_problem, row, GLP_DB, constraint.range.lower - constant, constraint.range.upper - constant);
          break;
        case VariableRange::Fixed:
          glp_set_row_bnds(m_problem, row, GLP_FX, constraint.range.lower - constant, constraint.range.upper - constant);
          break;
      }

      auto linear_terms = constraint.expression.linear_terms();

      for (auto & term : linear_terms) {
        assert(term.coefficient != 0.0);
        int col = static_cast<int>(to_index(term.variable) + 1);

        row_indices.push_back(row);
        col_indices.push_back(col);
        coefficients.push_back(term.coefficient);
      }

      ++row_index;
      ++row;
    }

    /*
     * matrix
     */

    assert(glp_check_dup(raw_constraints.size(), raw_variables.size(), coefficients.size() - 1, row_indices.data(), col_indices.data(), coefficients.data()) == 0);
    glp_load_matrix(m_problem, coefficients.size() - 1, row_indices.data(), col_indices.data(), coefficients.data());

    if (!config.problem_output.empty()) {
      glp_write_lp(m_problem, nullptr, config.problem_output.string().c_str());
    }

    /*
     * solve
     */

    if (config.use_mip) {
      glp_iocp parameters;
      glp_init_iocp(&parameters);

      parameters.msg_lev = config.verbose ? GLP_MSG_ALL : GLP_MSG_OFF;
      parameters.presolve = config.presolve ? GLP_ON : GLP_OFF;
      parameters.tm_lim = (config.timeout != std::chrono::milliseconds::max()) ? config.timeout.count() : std::numeric_limits<int>::max();

      int ret = glp_intopt(m_problem, &parameters);

      if (!config.solution_output.empty()) {
        glp_print_mip(m_problem, config.solution_output.string().c_str());
      }

      if (ret == 0) {
        return to_solver_status(glp_mip_status(m_problem));
      }
    } else {
      glp_smcp parameters;
      glp_init_smcp(&parameters);

      parameters.msg_lev = config.verbose ? GLP_MSG_ALL : GLP_MSG_OFF;
      parameters.presolve = config.presolve ? GLP_ON : GLP_OFF;
      parameters.tm_lim = (config.timeout != std::chrono::milliseconds::max()) ? config.timeout.count() : std::numeric_limits<int>::max();

      int ret = glp_simplex(m_problem, &parameters);

      if (!config.solution_output.empty()) {
        glp_print_sol(m_problem, config.solution_output.string().c_str());
      }

      if (ret == 0) {
        return to_solver_status(glp_get_status(m_problem));
      }
    }

    return SolverStatus::Error;
  }

}


