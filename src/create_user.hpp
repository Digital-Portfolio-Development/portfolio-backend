#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <userver/components/component_list.hpp>

namespace portfolio::user::reqister {
  std::string RegisterValidation(const std::string &key, const std::string &value);
  void AppendRegisterUser(userver::components::ComponentList &component_list);
} // namespace portfolio::user::register