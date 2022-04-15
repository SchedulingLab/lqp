#include <lqp/Solver.h>

#include <cassert>
#include <cstdio>

#include "solvers/GlpkSolver.h"

namespace lqp {

  Solver::~Solver() = default;

  std::vector<Problem::Variable> Solver::variables(const Problem& problem) {
    return problem.m_variables;
  }

  std::vector<Problem::Constraint> Solver::constraints(const Problem& problem) {
    return problem.m_constraints;
  }

  Problem::Objective Solver::objective(const Problem& problem) {
    return problem.m_objective;
  }


  std::unique_ptr<Solver> make_solver(SolverImplementation implementation) {
    switch (implementation) {
      case SolverImplementation::Null:
        return std::make_unique<NullSolver>();
      case SolverImplementation::Print:
        return std::make_unique<PrintSolver>();
      case SolverImplementation::Glpk:
        return std::make_unique<GlpkSolver>();
    }

    return std::make_unique<NullSolver>();
  }

  bool NullSolver::available() const {
    return false;
  }

  SolverStatus NullSolver::solve([[maybe_unused]] const Problem& problem, [[maybe_unused]] const SolverConfig& config) {
    return SolverStatus::Error;
  }

  bool PrintSolver::available() const {
    return true;
  }

  namespace {
    void print_expr(const Problem& problem, const QExpr& expr) {
      bool first = true;

      double constant = expr.constant();

      if (constant != 0.0) {
        std::printf("%g", constant);
        first = false;
      }

      auto linear_terms = expr.linear_terms();

      for (auto & term : linear_terms) {
        if (first) {
          first = false;
        } else {
          std::fputs(" + ", stdout);
        }

        std::printf("%g * %s", term.coefficient, problem.variable_name(term.variable).c_str());
      }

      // TODO: quadratic terms
    }

  }

  SolverStatus PrintSolver::solve(const Problem& problem, const SolverConfig& config) {
    auto raw_objective = objective(problem);

    switch (raw_objective.sense) {
      case Sense::Maximize:
        std::fputs("Maximize", stdout);
        break;
      case Sense::Minimize:
        std::fputs("Minimize", stdout);
        break;
    }

    if (!raw_objective.name.empty()) {
      std::printf(" '%s': ", raw_objective.name.c_str());
    } else {
      std::fputs(": ", stdout);
    }

    print_expr(problem, raw_objective.expression);
    std::puts("");

    auto raw_variables = variables(problem);
    auto raw_constraints = constraints(problem);

    for (auto & constraint : raw_constraints) {

      switch (constraint.range.type) {
        case VariableRange::LowerBounded:
          print_expr(problem, constraint.expression);
          std::printf(" >= %g", constraint.range.lower);
          break;
        case VariableRange::UpperBounded:
          print_expr(problem, constraint.expression);
          std::printf(" <= %g", constraint.range.upper);
          break;
        case VariableRange::Bounded:
          std::printf("%g <= ", constraint.range.lower);
          print_expr(problem, constraint.expression);
          std::printf(" <= %g", constraint.range.upper);
          break;
        case VariableRange::Fixed:
          print_expr(problem, constraint.expression);
          std::printf(" == %g", constraint.range.lower);
          break;
        default:
          assert(false);
          break;
      }

      if (!constraint.name.empty()) {
        std::printf(" (%s)", constraint.name.c_str());
      }

      std::puts("");
    }

    return SolverStatus::Error;
  }

}


