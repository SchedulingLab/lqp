// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023-2024 Julien Bernard

// clang-format off: main header
#include <lqp/Solver.h>
// clang format on

#include <cassert>
#include <cstdio>

namespace lqp {
  /*
   * Solver
   */

  Solver::~Solver() = default;

  std::vector<Problem::Variable> Solver::variables(const Problem& problem)
  {
    return problem.m_variables;
  }

  std::vector<Problem::Constraint> Solver::constraints(const Problem& problem)
  {
    return problem.m_constraints;
  }

  Problem::Objective Solver::objective(const Problem& problem)
  {
    return problem.m_objective;
  }

  bool NullSolver::available() const
  {
    return false;
  }

  Solution NullSolver::solve([[maybe_unused]] const Problem& problem, [[maybe_unused]] const SolverConfig& config)
  {
    return { SolutionStatus::NotSolved };
  }

}
