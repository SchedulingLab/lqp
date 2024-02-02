// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023-2024 Julien Bernard
#ifndef LQP_INEQUALITY_H
#define LQP_INEQUALITY_H

#include "Api.h"
#include "Expr.h"

namespace lqp {

  enum class Operator {
    GreaterEqual,
    Equal,
    LessEqual,
  };

  struct LQP_API Inequality {
    QExpr expression;
    Operator op;
  };

  LQP_API Inequality operator<=(const QExpr& lhs, const QExpr& rhs);
  LQP_API Inequality operator>=(const QExpr& lhs, const QExpr& rhs);
  LQP_API Inequality operator==(const QExpr& lhs, const QExpr& rhs);

}

#endif // LQP_INEQUALITY_H
