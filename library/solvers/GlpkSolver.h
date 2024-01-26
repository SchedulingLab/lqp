// SPDX-License-Identifier: MIT
// Copyright (c) 2023-2024 Julien Bernard
#ifndef LQP_GLPK_SOLVER_H
#define LQP_GLPK_SOLVER_H

#include <glpk.h>

#include <lqp/Solver.h>

namespace lqp {

  class GlpkSolver : public Solver {
  public:
    GlpkSolver();
    ~GlpkSolver();

    bool available() const override;
    SolverResult solve(const Problem& problem, const SolverConfig& config) override;

  private:
    glp_prob *m_problem;
  };

}

#endif // LQP_GLPK_SOLVER_H
