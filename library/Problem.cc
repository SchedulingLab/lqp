#include <lqp/Problem.h>

#include <cassert>
#include <cmath>
#include <iostream>

#include <lqp/Instance.h>

namespace lqp {

  VariableId Problem::add_variable(VariableCategory category, std::string name) {
    Variable variable;

    variable.category = category;

    if (category == VariableCategory::Binary) {
      variable.range.type = VariableRange::Bounded;
      variable.range.lower = 0.0;
      variable.range.upper = 1.0;
    } else {
      variable.range.type = VariableRange::Unbounded;
    }

    variable.name = std::move(name);

    std::size_t index = m_variables.size();
    m_variables.push_back(std::move(variable));
    return VariableId{index};
  }

  VariableId Problem::add_variable(VariableCategory category, VariableRange range, std::string name) {
    Variable variable;

    variable.category = category;
    variable.range = range;
    variable.name = std::move(name);

    std::size_t index = m_variables.size();
    m_variables.push_back(std::move(variable));
    return VariableId{index};
  }

  ConstraintId Problem::add_constraint(Inequality inequality, std::string name) {
    Constraint constraint;

    constraint.expression = std::move(inequality.expression);

    switch (inequality.op) {
      case Operator::GreaterEqual:
        constraint.range = lower_bound(0.0);
        break;
      case Operator::Equal:
        constraint.range = fixed(0.0);
        break;
      case Operator::LessEqual:
        constraint.range = upper_bound(0.0);
        break;
    }

    constraint.name = std::move(name);

    std::size_t index = m_constraints.size();
    m_constraints.push_back(std::move(constraint));
    return ConstraintId{index};
  }

  void Problem::set_objective(Sense sense, const LExpr& expr, std::string name) {
    m_objective = { sense, expr, std::move(name) };
  }

  std::string Problem::variable_name(VariableId variable) const {
    std::size_t index = to_index(variable);
    assert(index < m_variables.size());

    if (!m_variables[index].name.empty()) {
      return m_variables[index].name;
    }

    return "v" + std::to_string(index);
  }

  bool Problem::is_linear() const {
    for (auto & constraint : m_constraints) {
      if (!constraint.expression.is_linear()) {
        return false;
      }
    }

    return true;
  }

  std::optional<Problem> Problem::linearize() const {
    if (is_linear()) {
      return *this;
    }

    Problem result;
    result.m_variables = m_variables;
    result.m_objective = m_objective;

    for (auto & constraint : m_constraints) {
      if (constraint.expression.is_linear()) {
        result.m_constraints.push_back(constraint);
        continue;
      }

      QExpr expression = constraint.expression.constant();
      auto linear_terms = constraint.expression.linear_terms();

      for (auto & term : linear_terms) {
        expression += term.coefficient * term.variable;
      }

      std::map<std::tuple<VariableId, VariableId>, VariableId> mapping;
      auto quadratic_terms = constraint.expression.quadratic_terms();

      for (auto & term : quadratic_terms) {
        auto v0 = term.variables[0];
        auto v1 = term.variables[1];

        auto c0 = m_variables[to_index(v0)].category;
        auto c1 = m_variables[to_index(v1)].category;

        if (c0 == VariableCategory::Binary && c1 == VariableCategory::Binary) {
          auto it = mapping.find(std::make_tuple(v0, v1));

          VariableId variable;

          if (it != mapping.end()) {
            variable = it->second;
          } else {
            auto variable = result.add_variable(VariableCategory::Binary);
            mapping.insert({ std::make_tuple(v0, v1), variable });
            result.add_constraint(variable >= v0 + v1 - 1);
            result.add_constraint(variable <= 0.5 * (v0 + v1));
          }

          expression += term.coefficient * variable;
        } else if ((c0 == VariableCategory::Binary && c1 == VariableCategory::Continuous) || (c1 == VariableCategory::Binary && c0 == VariableCategory::Continuous)) {
          if (c1 == VariableCategory::Binary && c0 == VariableCategory::Continuous) {
            std::swap(v0, v1);
          }

          auto range = m_variables[to_index(v1)].range;

          if (range.type != VariableRange::Bounded || range.lower != 0.0) {
            return std::nullopt;
          }

          auto it = mapping.find(std::make_tuple(v0, v1));

          VariableId variable;

          if (it != mapping.end()) {
            variable = it->second;
          } else {
            auto variable = result.add_variable(VariableCategory::Continuous, range);
            mapping.insert({ std::make_tuple(v0, v1), variable });
            result.add_constraint(variable <= range.upper * v0);
            result.add_constraint(variable <= v1);
            result.add_constraint(variable >= v1 - (1.0 - v0) * range.upper);
          }

          expression += term.coefficient * variable;
        } else {
          return std::nullopt;
        }
      }

      result.m_constraints.push_back({ expression, constraint.range, constraint.name });
    }

    return std::nullopt;
  }


  bool Problem::is_feasible(const Instance& instance) const {
    // 1. verify that the variables well defined

    std::size_t index = 0;

    for (auto & problem_variable : m_variables) {
      VariableId variable{index};
      double value = instance.value(variable);

      switch (problem_variable.category) {
        case VariableCategory::Binary:
          if (value != 0.0 && value != 1.0) {
            return false;
          }

          break;

        case VariableCategory::Integer:
          if (std::trunc(value) != value) {
            return false;
          }

          if (!problem_variable.range.has_value(value)) {
            return false;
          }

          break;

        case VariableCategory::Continuous:
          if (!problem_variable.range.has_value(value)) {
            return false;
          }

          break;
      }

      ++index;
    }

    // 2. Verify that the constraints are satisfied

    for (auto & problem_constraint : m_constraints) {
      double value = problem_constraint.expression.evaluate(instance);

      if (!problem_constraint.range.has_value(value)) {
        return false;
      }
    }

    return true;
  }

  double Problem::compute_objective_value(const Instance& instance) const {
    return m_objective.expression.evaluate(instance);
  }

  void Problem::print() const {
    switch (m_objective.sense) {
      case Sense::Maximize:
        std::fputs("Maximize", stdout);
        break;
      case Sense::Minimize:
        std::fputs("Minimize", stdout);
        break;
    }

    if (!m_objective.name.empty()) {
      std::printf(" '%s': ", m_objective.name.c_str());
    } else {
      std::fputs(": ", stdout);
    }

    print_expr(m_objective.expression);
    std::puts("");

    for (auto & constraint : m_constraints) {
      if (!constraint.name.empty()) {
        std::printf("(%s) ", constraint.name.c_str());
      }

      switch (constraint.range.type) {
        case VariableRange::LowerBounded:
          print_expr(constraint.expression);
          std::printf(" >= %g", constraint.range.lower);
          break;
        case VariableRange::UpperBounded:
          print_expr(constraint.expression);
          std::printf(" <= %g", constraint.range.upper);
          break;
        case VariableRange::Bounded:
          std::printf("%g <= ", constraint.range.lower);
          print_expr(constraint.expression);
          std::printf(" <= %g", constraint.range.upper);
          break;
        case VariableRange::Fixed:
          print_expr(constraint.expression);
          std::printf(" == %g", constraint.range.lower);
          break;
        default:
          assert(false);
          break;
      }

      std::puts("");
    }
  }

  void Problem::print_expr(const QExpr& expr) const {
    bool first = true;

    double constant = expr.constant();

    if (constant != 0.0) {
      std::printf("%g", constant);
      first = false;
    }

    auto linear_terms = expr.linear_terms();

    for (auto & term : linear_terms) {
      if (first) {
        first = false;
      } else {
        std::fputs(" + ", stdout);
      }

      std::printf("%g * %s", term.coefficient, variable_name(term.variable).c_str());
    }

    // TODO: quadratic terms

  }

}
