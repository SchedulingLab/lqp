#ifndef LQP_INSTANCE_H
#define LQP_INSTANCE_H

#include <map>

#include "Api.h"
#include "Variable.h"

namespace lqp {

  class LQP_API Instance {
  public:

    void set_value(VariableId variable, double value);
    double value(VariableId variable) const;

  private:
    std::map<VariableId, double> m_values;
  };

}

#endif // LQP_INSTANCE_H
