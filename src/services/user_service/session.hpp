#include <jwt-cpp/jwt.h>

#include <core/include/userver/server/handlers/http_handler_json_base.hpp>
#include <core/include/userver/components/component_list.hpp>
#include <postgresql/include/userver/storages/postgres/cluster.hpp>
#include <postgresql/include/userver/storages/postgres/component.hpp>
#include <shared/include/userver/crypto/hash.hpp>

// for cookies (SetHttpOnly, SetSameSite)
#include <core/include/userver/server/http/http_response_cookie.hpp>

namespace portfolio::session {
  class Session final : public userver::server::handlers::HttpHandlerJsonBase {
   public:
    static constexpr std::string_view kName = "handler-user-login";

    Session(const userver::components::ComponentConfig &config,
            const userver::components::ComponentContext &context);

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest &request,
        const userver::formats::json::Value &request_json,
        userver::server::request::RequestContext &) const override;

    static std::string CheckRequestData(
        const userver::formats::json::Value &request_json);

   private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
  };

  void AppendSession(userver::components::ComponentList &component_list);
} // namespace portfolio::session