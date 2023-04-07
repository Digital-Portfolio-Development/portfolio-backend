#include <iostream>
#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/transaction.hpp>

namespace portfolio::user::login {
  std::string LoginValidation(const std::string &key, const std::string &value);
  bool ComparePassword(
      const userver::storages::postgres::ResultSet &transaction,
      const std::string &password);
  void AppendLoginUser(userver::components::ComponentList &component_list);
} // namespace portfolio::user::login