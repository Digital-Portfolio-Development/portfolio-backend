#include "create_session.hpp"
#include "userver/clients/dns/component.hpp"

#include <jwt-cpp/jwt.h>
#include <core/include/userver/server/handlers/http_handler_base.hpp>
#include <postgresql/include/userver/storages/postgres/cluster.hpp>
#include <postgresql/include/userver/storages/postgres/component.hpp>
#include <userver/crypto/hash.hpp>
#include <shared/include/userver/utils/regex.hpp>

// for cookies (SetHttpOnly, SetSameSite)
#include <core/include/userver/server/http/http_response_cookie.hpp>

namespace portfolio::user {
  namespace {
    class CreateSession final : public userver::server::handlers::HttpHandlerBase {
    public:
      static constexpr std::string_view kName = "handler-user-login";

      CreateSession(const userver::components::ComponentConfig &config,
                    const userver::components::ComponentContext &context)
          : HttpHandlerBase(config, context), pg_cluster_(context
                      .FindComponent<userver::components::Postgres>("portfolio-db")
                      .GetCluster()) {}

      std::string HandleRequestThrow(
          const userver::server::http::HttpRequest &request,
          userver::server::request::RequestContext &) const override {
        std::string response, error;

        userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
            "login_transaction",
            userver::storages::postgres::ClusterHostType::kMaster,
            {}
        );

        userver::formats::json::Value body = userver::formats::json::FromString(request.RequestBody());

        // Validation
        for (auto [key, value] : userver::formats::json::Items(body)) {
          error = portfolio::user::ValidateLogin(key, value.ConvertTo<std::string>());
          if (!error.empty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            return error;
          }
        }

        std::string email = body["email"].As<std::string>();
        std::string password = body["password"].As<std::string>();

        userver::storages::postgres::ResultSet candidate =
            transaction.Execute("SELECT * from users WHERE email = $1", email);
        if (candidate.IsEmpty() ||
            userver::crypto::hash::Sha256(password) != candidate[0]["password"].As<std::string>()) {
          request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
          return "Invalid email or password\n";
        }

        std::int32_t user_id = candidate[0]["user_id"].As<std::int32_t>();
        std::string token = jwt::create()
            .set_issuer("auth0")
            .set_type("JWS")
            .set_payload_claim("token", jwt::claim(email))
            .sign(jwt::algorithm::hs256{"secret-key"});

        userver::server::http::Cookie cookie1("user_id", std::to_string(user_id));
        userver::server::http::Cookie cookie2("access_key", token);
        cookie1.SetHttpOnly();
        cookie1.SameSite();
        cookie1.SetPath("/");
        cookie2.SetHttpOnly();
        cookie2.SameSite();
        cookie2.SetPath("/");

        // work only with SSL/HTTPS
        // cookie1.SetSecure();
        // cookie2.SetSecure();

        request.GetHttpResponse().SetCookie(cookie1);
        request.GetHttpResponse().SetCookie(cookie2);

        return "Success auth!\n";
      }

      userver::storages::postgres::ClusterPtr pg_cluster_;
    };
  } // namespace

  std::string ValidateLogin(const std::string &key, const std::string &value) {
    std::string error;

    if (key == "email") {
      userver::utils::regex pattern = userver::utils::regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
      if (!userver::utils::regex_match(value, pattern)) {
        error = "Email is invalid";
      }
    } else if (key == "password") {
      if (value.length() < 5) {
        error = "Password is too short";
      }
    }

    return error;
  }

  void AppendLoginUser(userver::components::ComponentList &component_list) {
    component_list.Append<CreateSession>();
  }
} // namespace portfolio::user
