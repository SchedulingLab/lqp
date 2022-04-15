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

}


