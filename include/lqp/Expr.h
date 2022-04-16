#ifndef LQP_EXPR_H
#define LQP_EXPR_H

#include <vector>

#include "Api.h"
#include "Variable.h"

namespace lqp {

  class Instance;

  struct LQP_API ExprLinearTerm {
    double coefficient;
    VariableId variable;
  };

  struct LQP_API ExprQuadraticTerm {
    double coefficient;
    VariableId variables[2];
  };

  // linear expression
  class LQP_API LExpr {
  public:
    LExpr();
    LExpr(double constant);
    LExpr(VariableId variable);
    LExpr(double coefficient, VariableId variable);

    bool is_constant() const;

    double constant() const;
    double linear_coefficient(VariableId variable) const;

    std::vector<ExprLinearTerm> linear_terms() const;

    LExpr& operator+=(const LExpr& other);
    LExpr& operator-=(const LExpr& other);

    LExpr& operator*=(double factor);
    LExpr& operator/=(double factor);

    double evaluate(const Instance& instance) const;

  private:
    void normalize();

  private:
    friend class QExpr;

    double m_constant;
    std::vector<ExprLinearTerm> m_linear_terms;
  };

  inline
  LExpr operator+(const LExpr& lhs, const LExpr& rhs) {
    LExpr expr(lhs);
    expr += rhs;
    return expr;
  }

  inline
  LExpr operator+(VariableId rhs, VariableId lhs) {
    return LExpr(rhs) + LExpr(lhs);
  }

  inline
  LExpr operator+(double rhs, const LExpr& lhs) {
    return LExpr(rhs) + lhs;
  }

  inline
  LExpr operator+(const LExpr& rhs, double lhs) {
    return rhs + LExpr(lhs);
  }

  inline
  LExpr operator-(const LExpr& lhs, const LExpr& rhs) {
    LExpr expr(lhs);
    expr -= rhs;
    return expr;
  }

  inline
  LExpr operator-(VariableId rhs, VariableId lhs) {
    return LExpr(rhs) - LExpr(lhs);
  }

  inline
  LExpr operator-(double rhs, const LExpr& lhs) {
    return LExpr(rhs) - lhs;
  }

  inline
  LExpr operator-(const LExpr& rhs, double lhs) {
    return rhs - LExpr(lhs);
  }

  inline
  LExpr operator*(double lhs, const LExpr& rhs) {
    LExpr expr(rhs);
    expr *= lhs;
    return expr;
  }

  inline
  LExpr operator*(const LExpr& lhs, double rhs) {
    LExpr expr(lhs);
    expr *= rhs;
    return expr;
  }

  inline
  LExpr operator/(const LExpr& lhs, double rhs) {
    LExpr expr(lhs);
    expr /= rhs;
    return expr;
  }

  inline
  LExpr operator*(double lhs, VariableId rhs) {
    return LExpr(lhs, rhs);
  }

  inline
  LExpr operator*(VariableId lhs, double rhs) {
    return LExpr(rhs, lhs);
  }


  class LQP_API QExpr {
  public:
    QExpr();
    QExpr(double constant);
    QExpr(VariableId variable);
    QExpr(const LExpr& expr);

    QExpr(VariableId variable1, VariableId variable2);
    QExpr(const LExpr& expr1, const LExpr& expr2);

    bool is_constant() const;
    bool is_linear() const;

    double constant() const;
    double linear_coefficient(VariableId variable) const;
    double quadratic_coefficient(VariableId variable1, VariableId variable2) const;

    std::vector<ExprLinearTerm> linear_terms() const;
    std::vector<ExprQuadraticTerm> quadratic_terms() const;

    QExpr& operator+=(const QExpr& other);
    QExpr& operator-=(const QExpr& other);

    QExpr& operator*=(double factor);
    QExpr& operator/=(double factor);

    double evaluate(const Instance& instance) const;

  private:
    void normalize();

  private:
    double m_constant;
    std::vector<ExprLinearTerm> m_linear_terms;
    std::vector<ExprQuadraticTerm> m_quadratic_terms;
  };


  inline
  QExpr operator+(const QExpr& lhs, const QExpr& rhs) {
    QExpr expr(lhs);
    expr += rhs;
    return expr;
  }

  inline
  QExpr operator-(const QExpr& lhs, const QExpr& rhs) {
    QExpr expr(lhs);
    expr -= rhs;
    return expr;
  }

  inline
  QExpr operator*(double lhs, const QExpr& rhs) {
    QExpr expr(rhs);
    expr *= lhs;
    return expr;
  }

  inline
  QExpr operator*(const QExpr& lhs, double rhs) {
    QExpr expr(lhs);
    expr *= rhs;
    return expr;
  }

  inline
  QExpr operator/(const QExpr& lhs, double rhs) {
    QExpr expr(lhs);
    expr /= rhs;
    return expr;
  }

  inline
  QExpr operator*(VariableId lhs, VariableId rhs) {
    return QExpr(lhs, rhs);
  }

  inline
  QExpr operator*(VariableId lhs, const LExpr& rhs) {
    return QExpr(LExpr(lhs), rhs);
  }

  inline
  QExpr operator*(const LExpr& lhs, VariableId rhs) {
    return QExpr(lhs, LExpr(rhs));
  }

  inline
  QExpr operator*(const LExpr& lhs, const LExpr& rhs) {
    return QExpr(lhs, rhs);
  }

}

#endif // LQP_EXPR_H
