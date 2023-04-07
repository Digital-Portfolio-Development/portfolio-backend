#include "create_session.hpp"
#include "userver/clients/dns/component.hpp"

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/crypto/base64.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/regex.hpp>
#include <jwt-cpp/jwt.h>

// for cookies (SetHttpOnly, SetSameSite)
#include <userver/server/http/http_response_cookie.hpp>

namespace portfolio::user::login {
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
        for (auto x : userver::formats::json::Items(body)) {
          error = portfolio::user::login::LoginValidation(
              x.key,
              x.value.ConvertTo<std::string>()
          );
          if (!error.empty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            return error;
          }
        }

        std::string email = body["email"].ConvertTo<std::string>();
        std::string password = body["password"].ConvertTo<std::string>();

        auto candidate = transaction.Execute("SELECT * from users WHERE email = $1", email);
        if (candidate.IsEmpty() || !portfolio::user::login::ComparePassword(candidate, password)) {
          request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
          return "Invalid email or password";
        }

        std::string token = jwt::create()
            .set_issuer("auth0")
            .set_type("JWS")
            .set_payload_claim("token", jwt::claim(password))
            .sign(jwt::algorithm::hs256{"secret-key"});

        userver::server::http::Cookie cookie("token", token);
        cookie.SetHttpOnly();
        cookie.SameSite();

        return "Success auth";
      }

      userver::storages::postgres::ClusterPtr pg_cluster_;
    };
  } // namespace

  std::string LoginValidation(const std::string &key, const std::string &value) {
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

  bool ComparePassword(
      const userver::storages::postgres::ResultSet &candidate,
      const std::string &password) {

    bool flag = true;

    std::string hash_password = candidate[0]["password"].As<std::string>();
    std::cout << "HASH_PASSWORD:\n" + hash_password + "\n\n\n\n\n\n";
    std::string unhash_password = userver::crypto::base64::Base64UrlDecode(hash_password);
    std::cout << "UNHASH_PASSWORD:\n" + unhash_password + "\n\n\n\n\n\n";

    if (password != userver::crypto::base64::Base64Decode(hash_password)) {
      flag = false;
    }

    return flag;
  }

  void AppendLoginUser(userver::components::ComponentList &component_list) {
    component_list.Append<CreateSession>();
  }
} // namespace portfolio::user::login
