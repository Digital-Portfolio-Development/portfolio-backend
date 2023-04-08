#include <iostream>
#include <string>
#include <string_view>

#include <core/include/userver/components/component_list.hpp>
#include <postgresql/include/userver/storages/postgres/transaction.hpp>

namespace portfolio::user {
  std::string LoginValidation(const std::string &key, const std::string &value);
  bool ComparePassword(
      const userver::storages::postgres::ResultSet &transaction,
      const std::string &password);
  void AppendLoginUser(userver::components::ComponentList &component_list);
} // namespace portfolio::user