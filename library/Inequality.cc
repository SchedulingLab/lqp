// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023-2024 Julien Bernard
#include <lqp/Inequality.h>

namespace lqp {

  Inequality operator<=(const QExpr& lhs, const QExpr& rhs) {
    return { lhs - rhs, Operator::LessEqual };
  }

  Inequality operator>=(const QExpr& lhs, const QExpr& rhs) {
    return { lhs - rhs, Operator::GreaterEqual };
  }

  Inequality operator==(const QExpr& lhs, const QExpr& rhs) {
    return { lhs - rhs, Operator::Equal };
  }

}
