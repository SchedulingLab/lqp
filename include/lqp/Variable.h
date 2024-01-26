// SPDX-License-Identifier: MIT
// Copyright (c) 2023-2024 Julien Bernard
#ifndef LQP_VARIABLE_H
#define LQP_VARIABLE_H

#include <cstddef>

#include "Api.h"

namespace lqp {

  struct LQP_API VariableId {
    std::size_t index;
  };

  constexpr
  bool operator==(VariableId lhs, VariableId rhs) {
    return lhs.index == rhs.index;
  }

  constexpr
  bool operator<(VariableId lhs, VariableId rhs) {
    return lhs.index < rhs.index;
  }

  constexpr
  bool operator>(VariableId lhs, VariableId rhs) {
    return lhs.index > rhs.index;
  }

  constexpr
  std::size_t to_index(VariableId variable) {
    return variable.index;
  }


  enum class VariableCategory {
    Continuous,
    Integer,
    Binary,
  };

  struct LQP_API VariableRange {
    enum Type {
      Unbounded,
      LowerBounded,
      UpperBounded,
      Bounded,
      Fixed,
    };

    Type type;
    double lower;
    double upper;

    bool has_value(double value) const;
  };

  LQP_API VariableRange upper_bound(double value);
  LQP_API VariableRange lower_bound(double value);
  LQP_API VariableRange bounds(double lower, double upper);
  LQP_API VariableRange fixed(double value);

}

#endif // LQP_VARIABLE_H
