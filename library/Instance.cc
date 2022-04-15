#include <lqp/Instance.h>


namespace lqp {

  void Instance::set_value(VariableId variable, double value) {
    m_values[variable] = value;
  }

  double Instance::value(VariableId variable) const {
    auto it = m_values.find(variable);

    if (it != m_values.end()) {
      return it->second;
    }

    return 0.0;
  }


}

