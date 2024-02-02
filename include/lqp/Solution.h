// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023-2024 Julien Bernard
#ifndef LQP_SOLUTION_H
#define LQP_SOLUTION_H

#include <map>

#include "Api.h"
#include "Variable.h"

namespace lqp {

  enum class SolutionStatus {
    Error,
    Optimal,
    Feasible,
    Infeasible,
    NoFeasibleSolution,
    UnboundedSolution,
    Undefined,
    NotSolved,
  };

  class LQP_API Solution {
  public:
    Solution(SolutionStatus status);

    SolutionStatus status() const;

    bool empty() const;
    void clear();

    void set_value(VariableId variable, double value);
    double value(VariableId variable) const;

  private:
    SolutionStatus m_status = SolutionStatus::NotSolved;
    std::map<VariableId, double> m_values;
  };

}

#endif // LQP_SOLUTION_H
