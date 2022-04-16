#include "GurobiSolver.h"

#if LQP_HAS_GUROBI

namespace lqp {

  GurobiSolver::GurobiSolver()
  : m_model(m_env)
  {
  }

  bool GurobiSolver::available() const {
    // TODO
    return true;
  }

  SolverResult GurobiSolver::solve(const Problem& problem, const SolverConfig& config) {
    // TODO
    return SolverStatus::NotSolved;
  }

}

#endif
