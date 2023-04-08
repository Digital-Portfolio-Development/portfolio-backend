#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <core/include/userver/components/component_list.hpp>

namespace portfolio::user {
  std::string RegisterValidation(const std::string &key, const std::string &value);
  void AppendRegisterUser(userver::components::ComponentList &component_list);
} // namespace portfolio::user