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

namespace portfolio::base {
  class Base : public userver::server::handlers::HttpHandlerJsonBase {
   public:
    Base(const userver::components::ComponentConfig &config,
            const userver::components::ComponentContext &context);

    [[nodiscard]] userver::formats::json::Value CreateObject(
        const userver::server::http::HttpRequest &request,
        const std::string &target,
        const userver::storages::postgres::Query &insert_value) const;

    static std::string CheckAndHash(
        userver::storages::postgres::Transaction &transaction,
        userver::formats::json::Value& body) ;

   private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
  };
}

#endif
