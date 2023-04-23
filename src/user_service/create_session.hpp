#include <iostream>
#include <string>
#include <string_view>

#include <core/include/userver/components/component_list.hpp>
#include <postgresql/include/userver/storages/postgres/transaction.hpp>

namespace portfolio::user {
  std::string ValidateLogin(const std::string &key, const std::string &value);
  void AppendLoginUser(userver::components::ComponentList &component_list);
} // namespace portfolio::user