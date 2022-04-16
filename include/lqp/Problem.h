#ifndef LQP_PROBLEM_H
#define LQP_PROBLEM_H

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

  enum class Sense {
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

    void print() const;

  private:
    void print_expr(const QExpr& expr) const;

  private:
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

    std::vector<Variable> m_variables;
    std::vector<Constraint> m_constraints;

    Objective m_objective;

  };

}

#endif // LQP_PROBLEM_H
