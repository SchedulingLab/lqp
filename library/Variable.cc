// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023-2024 Julien Bernard

// clang-format off: main header
#include <lqp/Variable.h>
// clang-format on

#include <cassert>


namespace lqp {

  bool VariableRange::has_value(double value) const
  {
    switch (type) {
      case Unbounded:
        return true;
      case LowerBounded:
        return lower <= value;
      case UpperBounded:
        return value <= upper;
      case Bounded:
        return lower <= value && value <= upper;
      case Fixed:
        return value == lower && value == upper;
    }

    assert(false);
    return true;
  }

  VariableRange upper_bound(double value)
  {
    return { VariableRange::UpperBounded, value, value };
  }

  VariableRange lower_bound(double value)
  {
    return { VariableRange::LowerBounded, value, value };
  }

  VariableRange bounds(double lower, double upper)
  {
    return { VariableRange::Bounded, lower, upper };
  }

  VariableRange fixed(double value)
  {
    return { VariableRange::Fixed, value, value };
  }

}
