#include "base.hpp"

namespace portfolio::base {
  Base::Base(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context), pg_cluster_(context
              .FindComponent<userver::components::Postgres>("portfolio-db")
              .GetCluster()) {}

  userver::formats::json::Value Base::CreateObject(
    const userver::server::http::HttpRequest &request,
    const std::string &target,
      const userver::storages::postgres::Query &insert_value
  ) const {
    std::string user_id = request.GetCookie("user_id");
    if (target != "user") {
      if (user_id.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        return utils::ResponseMessage("Unauthorized");
      }
    }

    userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
        fmt::format("create_{}_transaction", target),
        userver::storages::postgres::ClusterHostType::kMaster,
        {}
    );

    auto body = userver::formats::json::FromString(request.RequestBody());
    if (target == "user") {
      std::string error = CheckAndHash(transaction, body);
      if (!error.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return utils::ResponseMessage(error);
      }
    }

    // from https://userver.tech/da/dd0/classstorages_1_1postgres_1_1ParameterStore.html
    userver::storages::postgres::ParameterStore parameters;
    if (target != "user") {
      parameters.PushBack(std::stoi(user_id));
    }

    for (const auto& [key, value] : userver::formats::json::Items(body)) {
      if (value.IsInt()) {
        parameters.PushBack(value.As<int>());
      } else if (value.IsString()) {
        parameters.PushBack(value.As<std::string>());
      } else if (value.IsBool()) {
        parameters.PushBack(value.As<bool>());
      }
//      else if (value.IsArray()) {
//        parameters.PushBack(value.As<std::vector<std::string>>());
//      }
    }

    auto res = transaction.Execute(insert_value, parameters);

    if (res.RowsAffected()) {
      transaction.Commit();
      request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
      return utils::ResponseMessage(fmt::format("{} was created!", target));
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return utils::ResponseMessage("Unknown error");
  }

  std::string Base::CheckAndHash(
      userver::storages::postgres::Transaction &transaction,
      userver::formats::json::Value& body) {
    userver::storages::postgres::ResultSet candidate =
        transaction.Execute(
            "SELECT * from users WHERE username = $1",
            body["username"].As<std::string>()
        );
    if (!candidate.IsEmpty()) return "this username is already in use";

    if (!body["password"].As<std::string>().empty()) {
      userver::formats::json::ValueBuilder v{body};
      v["password"] = userver::crypto::hash::Sha256(body["password"].As<std::string>());
      body = v.ExtractValue();
    }

    return "";
  }
}
