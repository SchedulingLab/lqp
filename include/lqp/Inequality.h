#ifndef LQP_INEQUALITY_H
#define LQP_INEQUALITY_H

#include "Expr.h"

namespace lqp {

  enum class Operator {
    GreaterEqual,
    Equal,
    LessEqual,
  };

  struct Inequality {
    QExpr expression;
    Operator op;
  };

  Inequality operator<=(const QExpr& lhs, const QExpr& rhs);
  Inequality operator>=(const QExpr& lhs, const QExpr& rhs);
  Inequality operator==(const QExpr& lhs, const QExpr& rhs);

}

#endif // LQP_INEQUALITY_H
