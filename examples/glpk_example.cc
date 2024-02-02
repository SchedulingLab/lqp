// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023-2024 Julien Bernard
#include <cassert>
#include <cstdio>

#include <lqp/GlpkSolver.h>
#include <lqp/Problem.h>
#include <lqp/Solution.h>

int main() {
  lqp::Problem problem;

  auto x_1 = problem.add_variable(lqp::VariableCategory::Continuous, lqp::lower_bound(0.0), "x_1");
  auto x_2 = problem.add_variable(lqp::VariableCategory::Continuous, lqp::lower_bound(0.0), "x_2");
  auto x_3 = problem.add_variable(lqp::VariableCategory::Continuous, lqp::lower_bound(0.0), "x_3");

  problem.add_constraint(x_1 + x_2 + x_3 <= 100.0, "p");
  problem.add_constraint(10 * x_1 + 4 * x_2 + 5 * x_3 <= 600.0, "q");
  problem.add_constraint(2 * x_1 + 2 * x_2 + 6 * x_3 <= 300.0, "r");

  problem.set_objective(lqp::Sense::Maximize, 10 * x_1 + 6 * x_2 + 4 * x_3, "z");
  problem.print();

  lqp::GlpkSolver solver;

  lqp::SolverConfig config;
  config.problem_output = "problem.txt";
  config.solution_output = "solution.txt";

  auto solution = solver.solve(problem, config);
  assert(solution.status() == lqp::SolutionStatus::Optimal);

  std::printf("x_1 = %g, x_2 = %g, x_3 = %g\n", solution.value(x_1), solution.value(x_2), solution.value(x_3));

  return 0;
}
