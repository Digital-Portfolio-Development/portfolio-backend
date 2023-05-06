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
#include <userver/storages/postgres/parameter_store.hpp>
#include <shared/include/userver/crypto/hash.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/utils/text.hpp>

using namespace userver::server::http;

namespace portfolio::base {
  class Base : public userver::server::handlers::HttpHandlerJsonBase {
   public:
    Base(const userver::components::ComponentConfig &config,
            const userver::components::ComponentContext &context);

    [[nodiscard]] userver::formats::json::Value CreateObject(
        const userver::server::http::HttpRequest &request,
        const std::string &target,
        const userver::storages::postgres::Query &insert_value) const;

    template<typename TargetStruct>
    [[nodiscard]] userver::formats::json::Value GetRows(
        const HttpRequest &request,
        const std::string &target,
        const std::vector<std::string> &target_fields) const {

      auto transaction = pg_cluster_->Begin(
          fmt::format("get_{}_transaction", target),
          userver::storages::postgres::ClusterHostType::kMaster,
          {}
      );

      auto rows = transaction.Execute(fmt::format("SELECT * FROM {}", target));
      auto iteration = rows.AsSetOf<TargetStruct>(userver::storages::postgres::kRowTag);

      userver::formats::json::ValueBuilder json_res;

      int project_i = 1;
      for (auto row : iteration) {
        for (int i = 0; i < target_fields.size(); i++) {
          json_res[project_i][target_fields[i]] = *((&row) + i);
        }
        project_i++;
      }

      request.SetResponseStatus(HttpStatus::kOk);
      return json_res.ExtractValue();
    }

    static std::string CheckAndHash(
        userver::storages::postgres::Transaction &transaction,
        userver::formats::json::Value& body) ;

   private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
  };
}

#endif
