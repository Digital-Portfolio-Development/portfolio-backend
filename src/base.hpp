#ifndef SRC_BASE_H
#define SRC_BASE_H

#include "utils/utils.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <fmt/format.h>

#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/components/component_list.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/result_set.hpp>
#include <userver/storages/postgres/parameter_store.hpp>
#include <shared/include/userver/crypto/hash.hpp>
#include <userver/formats/parse/common_containers.hpp>

namespace portfolio::base {
  class Base : public userver::server::handlers::HttpHandlerJsonBase {
   public:
    Base(const userver::components::ComponentConfig &config,
            const userver::components::ComponentContext &context);

    [[nodiscard]] userver::formats::json::Value CreateObject(
        const userver::server::http::HttpRequest &request,
        std::string_view target,
        const userver::storages::postgres::Query &insert_value) const;

    [[nodiscard]] userver::formats::json::Value UpdateObject(
        const userver::server::http::HttpRequest &request,
        std::string_view target,
        const userver::storages::postgres::Query &update_value) const;

    [[nodiscard]] userver::formats::json::Value DeleteObject(
        const userver::server::http::HttpRequest &request,
        std::string_view target,
        std::string_view target_table,
        std::string_view object_id) const;

    userver::storages::postgres::ResultSet GetAllTable(
        std::string_view target_table) const;

    [[nodiscard]] userver::formats::json::Value GetComments(
        std::string_view target_type,
        std::string_view target_id) const;

    template<typename T>
    [[nodiscard]] userver::formats::json::Value SearchAndGetObjects(
        std::string_view target_type,
        std::string_view target_table,
        std::string_view key_word
    ) const {
      userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
          fmt::format("search_get_{}_transaction", target_type),
          userver::storages::postgres::ClusterHostType::kMaster,
          {}
      );

      auto result_set = transaction.Execute(
          fmt::format("SELECT c.*, u.username, u.user_avatar FROM {} c\n"
                      "JOIN users u ON c.user_id = u.user_id\n"
                      "WHERE tags LIKE '%{}%'", target_table, key_word)
      );
      auto iter_result_set = result_set.AsSetOf<T>(userver::storages::postgres::kRowTag);

      return utils::CreateJsonResult(iter_result_set);
    }



    static std::string CheckAndHash(
        userver::storages::postgres::Transaction &transaction,
        userver::formats::json::Value& body);

   private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
  };
}

#endif
