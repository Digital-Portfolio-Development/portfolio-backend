#include "../../base.hpp"

namespace portfolio::comment {
  class Comment final : public base::Base {
   public:
    static constexpr std::string_view kName = "handler-comment";

    Comment(const userver::components::ComponentConfig &config,
            const userver::components::ComponentContext &context);

    // Data that doesn't come in the request body (like user_id)
    // MUST be at the beginning of the field list.
    // Data that can be NULL (like media)
    // MUST be at the end of the field list and
    // MUST be marked as DEFAULT in the value list.
    const userver::storages::postgres::Query kInsertValue {
        "INSERT INTO comments (user_id, target_id, target_type, text, media)"
        "VALUES ($1, $2, $3, $4, $5)",
        userver::storages::postgres::Query::Name{"comment_insert_value"},
    };

    const userver::storages::postgres::Query kUpdateValue {
        "UPDATE comments "
        "SET text = $1, media = DEFAULT)"
        "WHERE comment_id = $2",
        userver::storages::postgres::Query::Name{"comment_update_value"},
    };

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest &request,
        const userver::formats::json::Value &request_json,
        userver::server::request::RequestContext &) const override;

    static std::string CheckRequestData(
        const userver::formats::json::Value &request_json) ;
  };

  void AppendComment(userver::components::ComponentList &component_list);
}
