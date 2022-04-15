#ifndef LQP_VARIABLE_H
#define LQP_VARIABLE_H

#include <cstddef>

namespace lqp {

  struct VariableId {
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

  struct VariableRange {
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

  VariableRange upper_bound(double value);
  VariableRange lower_bound(double value);
  VariableRange bounds(double lower, double upper);
  VariableRange fixed(double value);

}

#endif // LQP_VARIABLE_H
