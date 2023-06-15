#include "base.hpp"

namespace portfolio::base {
  Base::Base(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context), pg_cluster_(context
              .FindComponent<userver::components::Postgres>("portfolio-db")
              .GetCluster()) {}

  userver::formats::json::Value Base::CreateObject(
      const userver::server::http::HttpRequest &request,
      const std::string_view target_type,
      const userver::storages::postgres::Query &insert_value
  ) const {
    std::string user_id = request.GetCookie("user_id");
    if (target_type != "user") {
      if (user_id.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        return utils::ResponseMessage("unauthorized");
      }
    }

    userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
        fmt::format("create_{}_transaction", target_type),
        userver::storages::postgres::ClusterHostType::kMaster,
        {}
    );

    auto body = userver::formats::json::FromString(request.RequestBody());
    if (target_type == "user") {
      std::string error = CheckAndHash(transaction, body);
      if (!error.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return utils::ResponseMessage(error);
      }
    }

    // from https://userver.tech/da/dd0/classstorages_1_1postgres_1_1ParameterStore.html
    userver::storages::postgres::ParameterStore parameters;
    if (target_type != "user") {
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
    }

    auto res = transaction.Execute(insert_value, parameters);

    if (res.RowsAffected()) {
      transaction.Commit();
      request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
      return utils::ResponseMessage(fmt::format("{} was created!", target_type));
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return utils::ResponseMessage("unknown error");
  }

  userver::formats::json::Value Base::UpdateObject(
      const userver::server::http::HttpRequest &request,
      const std::string_view target_type,
      const userver::storages::postgres::Query &update_value) const {
    std::string user_id = request.GetCookie("user_id");
    if (user_id.empty()) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
      return utils::ResponseMessage("unauthorized");
    }

    userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
        fmt::format("create_{}_transaction", target_type),
        userver::storages::postgres::ClusterHostType::kMaster,
        {}
    );

    auto body = userver::formats::json::FromString(request.RequestBody());
    userver::storages::postgres::ParameterStore parameters;

    for (const auto& [key, value] : userver::formats::json::Items(body)) {
      if (value.IsInt()) {
        parameters.PushBack(value.As<int>());
      } else if (value.IsString()) {
        parameters.PushBack(value.As<std::string>());
      } else if (value.IsBool()) {
        parameters.PushBack(value.As<bool>());
      }
    }

    auto res = transaction.Execute(update_value, parameters);

    if (res.RowsAffected()) {
      transaction.Commit();
      request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
      return utils::ResponseMessage(fmt::format("{} was updated!", target_type));
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return utils::ResponseMessage("unknown error");
  }

  userver::formats::json::Value Base::DeleteObject(
      const userver::server::http::HttpRequest &request,
      const std::string_view target_type,
      const std::string_view target_table,
      const std::string_view object_id) const {
    std::string user_id = request.GetCookie("user_id");
    if (user_id.empty()) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
      return utils::ResponseMessage("unauthorized");
    }

    auto transaction = pg_cluster_->Begin(
        fmt::format("post_{}_transaction", target_type),
        userver::storages::postgres::ClusterHostType::kMaster,
        {}
    );
    auto project = transaction.Execute(
        fmt::format("SELECT * FROM {} WHERE {}_id = $1", target_table, target_type),
        std::stoi(object_id.data())
    );
    if (project.IsEmpty()) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return utils::ResponseMessage(
          fmt::format("{} with this id doesn't exist", target_type)
      );
    }

    auto res = transaction.Execute(
        fmt::format("DELETE FROM {} WHERE {}_id = $1 AND user_id = $2", target_table, target_type),
        std::stoi(object_id.data()),
        std::stoi(user_id)
    );

    if (res.RowsAffected()) {
      transaction.Commit();
      request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
      return utils::ResponseMessage(fmt::format("{} was deleted!", target_type));
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return utils::ResponseMessage("unknown error");
  }

  userver::storages::postgres::ResultSet Base::GetAllTable(
      std::string_view target_table) const {
    userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
        fmt::format("get_{}_table_transaction", target_table),
        userver::storages::postgres::ClusterHostType::kMaster, {});

    const userver::storages::postgres::Query kGetValue {
        fmt::format("SELECT * FROM {}", target_table),
        userver::storages::postgres::Query::Name {"get_table"}
    };
    auto result_set = transaction.Execute(kGetValue);
    return result_set;
  }

  userver::formats::json::Value Base::GetComments(
      const std::string_view target_type,
      const std::string_view target_id
  ) const {
    userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
        fmt::format("search_get_{}_transaction", target_type),
        userver::storages::postgres::ClusterHostType::kMaster, {});
    auto result_set = transaction.Execute(
        fmt::format("SELECT c.*, u.username, u.user_avatar FROM comments c\n"
                    "JOIN users u ON c.user_id = u.user_id\n"
                    "WHERE (c.target_id = {} and c.target_type = '{}')",
                    std::stoi(target_id.data()), target_type)
    );
    auto iter_result_set = result_set.AsSetOf<utils::CommentTuple>(userver::storages::postgres::kRowTag);
    return utils::CreateJsonResult(iter_result_set);
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
