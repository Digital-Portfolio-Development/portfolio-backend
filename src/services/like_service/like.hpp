#ifndef PORTFOLIO_BACKEND_LIKE_HPP
#define PORTFOLIO_BACKEND_LIKE_HPP

namespace portfolio::like {

class Like final : public base::Base {
 public:
  static constexpr std::string_view kName = "handler-like";

  Like(const userver::components::ComponentConfig &config,
          const userver::components::ComponentContext &context);

  // Data that doesn't come in the request body (like user_id)
  // MUST be at the beginning of the field list.
  // Data that can be NULL (like media)
  // MUST be at the end of the field list and
  // MUST be marked as DEFAULT in the value list.
  const userver::storages::postgres::Query kInsertValue {
      "INSERT INTO projects "
      "(target_id, user_id, target_type, username, created_at)"
      "VALUES ($1, $2, $3, $4, $5)",
      userver::storages::postgres::Query::Name {"like_insert_value"}
  };

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest &request,
      const userver::formats::json::Value &request_json,
      userver::server::request::RequestContext &) const override;

  static std::string CheckRequestData(
      const userver::formats::json::Value &request_json);
};

void AppendProject(userver::components::ComponentList &component_list);
}

}

#endif
