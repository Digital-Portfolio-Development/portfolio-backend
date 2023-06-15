#include "../../base.hpp"

namespace portfolio::user {
  class User final : public base::Base {
   public:
    static constexpr std::string_view kName = "handler-user";

    User(const userver::components::ComponentConfig &config,
            const userver::components::ComponentContext &context);

    const userver::storages::postgres::Query kInsertValue {
        "INSERT INTO users (name, last_name, username, password)"
        "VALUES ($1, $2, $3, $4)",
        userver::storages::postgres::Query::Name{"user_register_insert_value"},
    };

    const userver::storages::postgres::Query kUpdateValue {
        "UPDATE users SET name = $2, last_name = $3, username = $4, email = $5, phone_number = $6, password = $7, profession = $8, key_skills = $9, about = $10, country = $11, city = $12, avatar = $13, banner = $14 WHERE user_id = $1",
        userver::storages::postgres::Query::Name{"user_update_value"},
    };

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest &request,
        const userver::formats::json::Value &request_json,
        userver::server::request::RequestContext &) const override;

    static std::string CheckRequestData(
        const userver::formats::json::Value &request_json);
  };

  void AppendUser(userver::components::ComponentList &component_list);
} // namespace portfolio::user
