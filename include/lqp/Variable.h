// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023-2024 Julien Bernard
#ifndef LQP_VARIABLE_H
#define LQP_VARIABLE_H

#include <cstddef>
#include <cstdint>

#include "Api.h"

namespace lqp {

  struct LQP_API VariableId {
    std::size_t index;
  };

  constexpr bool operator==(VariableId lhs, VariableId rhs)
  {
    return lhs.index == rhs.index;
  }

  constexpr bool operator<(VariableId lhs, VariableId rhs)
  {
    return lhs.index < rhs.index;
  }

  constexpr bool operator>(VariableId lhs, VariableId rhs)
  {
    return lhs.index > rhs.index;
  }

  constexpr std::size_t to_index(VariableId variable)
  {
    return variable.index;
  }

  enum class VariableCategory : uint8_t {
    Continuous,
    Integer,
    Binary,
  };

  struct LQP_API VariableRange {
    enum Type : uint8_t {
      Unbounded,
      LowerBounded,
      UpperBounded,
      Bounded,
      Fixed,
    };

    Type type = Type::Unbounded;
    double lower = 0.0;
    double upper = 0.0;

    bool has_value(double value) const;
  };

  LQP_API VariableRange upper_bound(double value);
  LQP_API VariableRange lower_bound(double value);
  LQP_API VariableRange bounds(double lower, double upper);
  LQP_API VariableRange fixed(double value);

}

#endif // LQP_VARIABLE_H
