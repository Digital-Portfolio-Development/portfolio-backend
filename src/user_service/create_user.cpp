#include "create_user.hpp"

#include <fmt/format.h>

#include <core/include/userver/clients/dns/component.hpp>
#include <core/include/userver/server/handlers/http_handler_base.hpp>
#include <core/include/userver/server/http/http_status.hpp>
#include <postgresql/include/userver/storages/postgres/cluster.hpp>
#include <postgresql/include/userver/storages/postgres/component.hpp>
#include <shared/include/userver/crypto/base64.hpp>
#include <shared/include/userver/utils/regex.hpp>

namespace portfolio::user {
  namespace {
    class CreateUser final : public userver::server::handlers::HttpHandlerBase {
    public:
      static constexpr std::string_view kName = "handler-user-register";

      CreateUser(const userver::components::ComponentConfig &config,
                 const userver::components::ComponentContext &context)
          : HttpHandlerBase(config, context), pg_cluster_(context
                        .FindComponent<userver::components::Postgres>("portfolio-db")
                        .GetCluster()) {}

      const userver::storages::postgres::Query kInsertValue {
        "INSERT INTO users (name, last_name, username, email, password)"
        "VALUES ($1, $2, $3, $4, $5)",
        userver::storages::postgres::Query::Name {"user_insert_value"},
      };

      std::string HandleRequestThrow(
          const userver::server::http::HttpRequest &request,
          userver::server::request::RequestContext &) const override {
        std::string error;

        userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
            "create_user_transaction",
            userver::storages::postgres::ClusterHostType::kMaster,
            {}
        );

        userver::formats::json::Value body = userver::formats::json::FromString(request.RequestBody());

        // Validation
        for (auto [key, value] : userver::formats::json::Items(body)) {
          error = portfolio::user::RegisterValidation(key, value.ConvertTo<std::string>());
          if (!error.empty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            return error;
          }
        }

        std::string name = body["name"].As<std::string>();
        std::string last_name = body["last_name"].As<std::string>();
        std::string username = body["username"].As<std::string>();
        std::string email = body["email"].As<std::string>();
        std::string password = body["password"].As<std::string>();

        userver::storages::postgres::ResultSet candidate =
            transaction.Execute("SELECT username from users WHERE username = $1", username);
        if (!candidate.IsEmpty()) {
          request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
          return "User with this username already exists";
        }

        std::string hash_password = userver::crypto::base64::Base64UrlEncode(password);

        auto res = transaction.Execute(kInsertValue, name, last_name, username, email, hash_password);

        if (res.RowsAffected()) {
          transaction.Commit();
          request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
          return fmt::format("User {} was created!", username);
        }

        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return "Unknown error";;
      }

      userver::storages::postgres::ClusterPtr pg_cluster_;
    };
  } // namespace

  std::string RegisterValidation(const std::string &key, const std::string &value) {
    std::string error;

    if (key == "name") {
      if (value.length() < 3) {
        error = "Name is too short";
      }
    } else if (key == "last_name") {
      if (value.length() < 3) {
        error = "Last name is too short";
      }
    } else if (key == "username") {
      if (value.length() < 5) {
        error = "Username is too short";
      }
    } else if (key == "email") {
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

  void AppendRegisterUser(userver::components::ComponentList &component_list) {
    component_list.Append<CreateUser>();
    component_list.Append<userver::clients::dns::Component>();
  }
} // namespace portfolio::user


