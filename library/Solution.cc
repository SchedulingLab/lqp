// SPDX-License-Identifier: MIT
// Copyright (c) 2023-2024 Julien Bernard
#include <lqp/Solution.h>


namespace lqp {
  Solution::Solution(SolutionStatus status)
  : m_status(status)
  {
  }

  SolutionStatus Solution::status() const
  {
    return m_status;
  }

  bool Solution::empty() const {
    return m_values.empty();
  }

  void Solution::clear() {
    m_values.clear();
  }

  void Solution::set_value(VariableId variable, double value) {
    m_values[variable] = value;
  }

  double Solution::value(VariableId variable) const {
    auto it = m_values.find(variable);

    if (it != m_values.end()) {
      return it->second;
    }

    return 0.0;
  }


}

