// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023-2024 Julien Bernard
#ifndef LQP_GLPK_SOLVER_H
#define LQP_GLPK_SOLVER_H

#include "Api.h"
#include "Solver.h"

namespace lqp {

  class LQP_API GlpkSolver : public Solver {
  public:
    bool available() const override;
    Solution solve(const Problem& problem, const SolverConfig& config) override;
  };

}

#endif // LQP_GLPK_SOLVER_H
