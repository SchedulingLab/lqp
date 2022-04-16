#ifndef LQP_SOLUTION_H
#define LQP_SOLUTION_H

#include <map>

#include "Api.h"
#include "Variable.h"

namespace lqp {

  class LQP_API Solution {
  public:
    bool empty() const;
    void clear();

    void set_value(VariableId variable, double value);
    double value(VariableId variable) const;

  private:
    std::map<VariableId, double> m_values;
  };

}

#endif // LQP_SOLUTION_H
