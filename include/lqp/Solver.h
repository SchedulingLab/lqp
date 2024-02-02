// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023-2024 Julien Bernard
#ifndef LQP_SOLVER_H
#define LQP_SOLVER_H

#include <chrono>
#include <filesystem>

#include "Api.h"
#include "Problem.h"
#include "Solution.h"

namespace lqp {

  struct LQP_API SolverConfig {
    bool use_mip = false;
    bool verbose = true;
    bool presolve = false;
    std::chrono::milliseconds timeout = std::chrono::milliseconds::max();
    std::filesystem::path problem_output;
    std::filesystem::path solution_output;
  };

  class LQP_API Solver {
  public:
    Solver() = default;
    virtual ~Solver();

    Solver(const Solver&) = delete;
    Solver& operator=(const Solver&) = delete;

    Solver(Solver&&) = default;
    Solver& operator=(Solver&&) = default;

    virtual bool available() const = 0;
    virtual Solution solve(const Problem& problem, const SolverConfig& config = SolverConfig()) = 0;

  protected:
    static std::vector<Problem::Variable> variables(const Problem& problem);
    static std::vector<Problem::Constraint> constraints(const Problem& problem);
    static Problem::Objective objective(const Problem& problem);
  };

  class LQP_API NullSolver : public Solver {
  public:
    bool available() const override;
    Solution solve(const Problem& problem, const SolverConfig& config) override;
  };

}

#endif // LQP_SOLVER_H
