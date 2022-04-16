#ifndef LQP_GUROBI_SOLVER_H
#define LQP_GUROBI_SOLVER_H

#include "config.h"

#if LQP_HAS_GUROBI

#include <gurobi_c++.h>

#include <lqp/Solver.h>

namespace lqp {

  class GurobiSolver : public Solver {
  public:
    GurobiSolver();

    bool available() const override;
    SolverStatus solve(const Problem& problem, const SolverConfig& config) override;

  private:
    GRBEnv m_env;
    GRBModel m_model;
  };

}

#else

#include <lqp/Solver.h>

namespace lqp {

  using GurobiSolver = NullSolver;

}

#endif

#endif // LQP_GUROBI_SOLVER_H
