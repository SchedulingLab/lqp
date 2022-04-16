#include <lqp/Expr.h>

#include <cassert>
#include <map>
#include <tuple>

#include <lqp/Solution.h>

namespace lqp {

  LExpr::LExpr()
  : m_constant(0.0)
  {
  }

  LExpr::LExpr(double constant)
  : m_constant(constant)
  {
  }

  LExpr::LExpr(VariableId variable)
  : m_constant(0.0)
  {
    m_linear_terms.push_back({ 1.0, variable });
  }

  LExpr::LExpr(double coefficient, VariableId variable)
  : m_constant(0.0)
  {
    m_linear_terms.push_back({ coefficient, variable });
  }

  bool LExpr::is_constant() const {
    return m_linear_terms.empty();
  }

  double LExpr::constant() const {
    return m_constant;
  }

  double LExpr::linear_coefficient(VariableId variable) const {
    for (auto & term : m_linear_terms) {
      if (term.variable == variable) {
        return term.coefficient;
      }
    }

    return 0.0;
  }

  std::vector<ExprLinearTerm> LExpr::linear_terms() const {
    return m_linear_terms;
  }

  LExpr& LExpr::operator+=(const LExpr& other) {
    if (&other == this) {
      operator*=(2.0);
      return *this;
    }

    m_constant += other.m_constant;

    if (!other.m_linear_terms.empty()) {
      m_linear_terms.insert(m_linear_terms.end(), other.m_linear_terms.begin(), other.m_linear_terms.end());
    }

    normalize();
    return *this;
  }

  LExpr& LExpr::operator-=(const LExpr& other) {
    if (&other == this) {
      m_constant = 0.0;
      m_linear_terms.clear();
      return *this;
    }

    LExpr expr(other);
    expr *= -1.0;
    return operator+=(expr);
  }

  LExpr& LExpr::operator*=(double factor) {
    if (factor == 0.0) {
      m_constant = 0.0;
      m_linear_terms.clear();
      return *this;
    }

    m_constant *= factor;

    for (auto & term : m_linear_terms) {
      term.coefficient *= factor;
    }

    return *this;
  }

  LExpr& LExpr::operator/=(double factor) {
    m_constant /= factor;

    for (auto& term : m_linear_terms) {
      term.coefficient /= factor;
    }

    return *this;
  }

  double LExpr::evaluate(const Solution& solution) const {
    double value = m_constant;

    for (auto& term : m_linear_terms) {
      value += term.coefficient * solution.value(term.variable);
    }

    return value;
  }

  void LExpr::normalize() {
    std::map<VariableId, double> linear_terms;

    for (auto & term : m_linear_terms) {
      linear_terms[term.variable] += term.coefficient;
    }

    m_linear_terms.clear();

    for (auto [ variable, coefficient ] : linear_terms) {
      if (coefficient != 0.0) {
        m_linear_terms.push_back({ coefficient, variable });
      }
    }
  }



  QExpr::QExpr()
  : m_constant(0.0)
  {
  }

  QExpr::QExpr(double constant)
  : m_constant(constant)
  {
  }

  QExpr::QExpr(VariableId variable)
  : m_constant(0.0)
  {
    m_linear_terms.push_back({ 1.0, variable });
  }

  QExpr::QExpr(const LExpr& expr)
  : m_constant(expr.m_constant)
  , m_linear_terms(expr.m_linear_terms)
  {
  }

  QExpr::QExpr(VariableId variable1, VariableId variable2)
  : m_constant(0.0)
  {
    if (variable1 > variable2) {
      std::swap(variable1, variable2);
    }

    m_quadratic_terms.push_back({ 1.0, { variable1, variable2 } });
  }

  QExpr::QExpr(const LExpr& expr1, const LExpr& expr2)
  : m_constant(expr1.m_constant * expr2.m_constant)
  {
    if (expr1.m_constant != 0.0 && !expr2.m_linear_terms.empty()) {
      for (auto & term : expr2.m_linear_terms) {
        m_linear_terms.push_back({ expr1.m_constant * term.coefficient, term.variable });
      }
    }

    if (expr2.m_constant != 0.0 && !expr1.m_linear_terms.empty()) {
      for (auto & term : expr1.m_linear_terms) {
        m_linear_terms.push_back({ expr2.m_constant * term.coefficient, term.variable });
      }
    }

    for (auto & term1 : expr1.m_linear_terms) {
      for (auto & term2 : expr2.m_linear_terms) {
        ExprQuadraticTerm result = { term1.coefficient * term2.coefficient, { term1.variable, term2.variable } };

        if (result.variables[0] < result.variables[1]) {
          std::swap(result.variables[0], result.variables[1]);
        }

        m_quadratic_terms.push_back(result);
      }
    }

    normalize();
  }

  bool QExpr::is_constant() const {
    return m_linear_terms.empty() && m_quadratic_terms.empty();
  }

  bool QExpr::is_linear() const {
    return m_quadratic_terms.empty();
  }

  double QExpr::constant() const {
    return m_constant;
  }

  double QExpr::linear_coefficient(VariableId variable) const {
    for (auto & term : m_linear_terms) {
      if (term.variable == variable) {
        return term.coefficient;
      }
    }

    return 0.0;
  }

  double QExpr::quadratic_coefficient(VariableId variable1, VariableId variable2) const {
    if (variable1 > variable2) {
      std::swap(variable1, variable2);
    }

    for (auto & term : m_quadratic_terms) {
      if (term.variables[0] == variable1 && term.variables[1] == variable2) {
        return term.coefficient;
      }
    }

    return 0.0;
  }

  std::vector<ExprLinearTerm> QExpr::linear_terms() const {
    return m_linear_terms;
  }

  std::vector<ExprQuadraticTerm> QExpr::quadratic_terms() const {
    return m_quadratic_terms;
  }

  QExpr& QExpr::operator+=(const QExpr& other) {
    if (&other == this) {
      operator*=(2.0);
      return *this;
    }

    m_constant += other.m_constant;

    if (!other.m_linear_terms.empty()) {
      m_linear_terms.insert(m_linear_terms.end(), other.m_linear_terms.begin(), other.m_linear_terms.end());
    }

    if (!other.m_quadratic_terms.empty()) {
      m_quadratic_terms.insert(m_quadratic_terms.end(), other.m_quadratic_terms.begin(), other.m_quadratic_terms.end());
    }

    normalize();
    return *this;
  }

  QExpr& QExpr::operator-=(const QExpr& other) {
    if (&other == this) {
      m_constant = 0.0;
      m_linear_terms.clear();
      m_quadratic_terms.clear();
      return *this;
    }

    QExpr expr(other);
    expr *= -1.0;
    return operator+=(expr);
  }

  QExpr& QExpr::operator*=(double factor) {
    if (factor == 0.0) {
      m_constant = 0.0;
      m_linear_terms.clear();
      m_quadratic_terms.clear();
      return *this;
    }

    m_constant *= factor;

    for (auto & term : m_linear_terms) {
      term.coefficient *= factor;
    }

    for (auto & term : m_quadratic_terms) {
      term.coefficient *= factor;
    }

    return *this;
  }

  QExpr& QExpr::operator/=(double factor) {
    m_constant /= factor;

    for (auto& term : m_linear_terms) {
      term.coefficient /= factor;
    }

    for (auto& term : m_quadratic_terms) {
      term.coefficient /= factor;
    }

    return *this;
  }

  double QExpr::evaluate(const Solution& solution) const {
    double value = m_constant;

    for (auto& term : m_linear_terms) {
      value += term.coefficient * solution.value(term.variable);
    }

    for (auto& term : m_quadratic_terms) {
      value += term.coefficient * solution.value(term.variables[0]) * solution.value(term.variables[1]);
    }

    return value;
  }

  void QExpr::normalize() {
    std::map<VariableId, double> linear_terms;

    for (auto & term : m_linear_terms) {
      linear_terms[term.variable] += term.coefficient;
    }

    m_linear_terms.clear();

    for (auto [ variable, coefficient ] : linear_terms) {
      if (coefficient != 0.0) {
        m_linear_terms.push_back({ coefficient, variable });
      }
    }

    std::map<std::tuple<VariableId, VariableId>, double> quadratic_terms;

    for (auto & term : m_quadratic_terms) {
      assert(term.variables[0] <= term.variables[1]);
      quadratic_terms[std::make_tuple(term.variables[0], term.variables[1])] += term.coefficient;
    }

    m_quadratic_terms.clear();

    for (auto [ variables, coefficient ] : quadratic_terms) {
      if (coefficient != 0.0) {
        m_quadratic_terms.push_back({ coefficient, { std::get<0>(variables), std::get<1>(variables) } });
      }
    }
  }

}
