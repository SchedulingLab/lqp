// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023-2024 Julien Bernard
#ifndef LQP_PROBLEM_H
#define LQP_PROBLEM_H

#include <cstdint>

#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

#include "Api.h"
#include "Expr.h"
#include "Inequality.h"
#include "Variable.h"

namespace lqp {

  class Solution;

  struct LQP_API ConstraintId {
    std::size_t index;
  };

  enum class Sense : uint8_t {
    Minimize,
    Maximize,
  };

  class LQP_API Problem {
  public:
    VariableId add_variable(VariableCategory category, std::string name = "");
    VariableId add_variable(VariableCategory category, VariableRange range, std::string name = "");

    ConstraintId add_constraint(Inequality inequality, std::string name = "");

    void set_objective(Sense sense, const LExpr& expr, std::string name = "");

    std::string variable_name(VariableId var) const;

    bool is_linear() const;
    std::optional<Problem> linearize() const;

    bool is_feasible(const Solution& solution) const;
    double compute_objective_value(const Solution& solution) const;

    void print_to(std::ostream& out) const;

  private:
    void print_expr_to(const QExpr& expr, std::ostream& out) const;

    friend class Solver;

    struct Variable {
      VariableCategory category;
      VariableRange range;
      std::string name;
    };

    struct Constraint {
      QExpr expression;
      VariableRange range;
      std::string name;
    };

    struct Objective {
      Sense sense;
      LExpr expression;
      std::string name;
    };

    bool linearize_constraint(const Constraint& constraint, Problem& result) const;

    std::vector<Variable> m_variables;
    std::vector<Constraint> m_constraints;

    Objective m_objective;
  };

  inline std::ostream& operator<<(std::ostream& out, const Problem& problem)
  {
    problem.print_to(out);
    return out;
  }

}

#endif // LQP_PROBLEM_H
