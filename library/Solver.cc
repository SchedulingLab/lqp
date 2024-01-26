// SPDX-License-Identifier: MIT
// Copyright (c) 2023-2024 Julien Bernard
#include <lqp/Solver.h>

#include <cassert>
#include <cstdio>

#include "solvers/GlpkSolver.h"
#include "solvers/GurobiSolver.h"

// #include "config.h"

namespace lqp {
  /*
   * SolverResult
   */

  SolverResult::SolverResult(SolverStatus status)
  : m_status(status)
  {
  }

  SolverResult::SolverResult(SolverStatus status, Solution solution)
  : m_status(status)
  , m_solution(std::move(solution))
  {
  }

  SolverStatus SolverResult::status() const {
    return m_status;
  }

  bool SolverResult::has_solution() const {
    return !m_solution.empty();
  }

  Solution SolverResult::solution() {
    return m_solution;
  }

  /*
   * Solver
   */

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
      case SolverImplementation::Gurobi:
        return std::make_unique<GurobiSolver>();
    }

    return std::make_unique<NullSolver>();
  }

  bool NullSolver::available() const {
    return false;
  }

  SolverResult NullSolver::solve([[maybe_unused]] const Problem& problem, [[maybe_unused]] const SolverConfig& config) {
    return SolverStatus::NotSolved;
  }

}
