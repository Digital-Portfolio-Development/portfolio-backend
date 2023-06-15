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
        "UPDATE comments SET text = $2, media = '' WHERE comment_id = $1",
        userver::storages::postgres::Query::Name{"comment_update_value"},
    }; // TODO: Add some more fields

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest &request,
        const userver::formats::json::Value &request_json,
        userver::server::request::RequestContext &) const override;

    static std::string CheckRequestData(
        const userver::formats::json::Value &request_json) ;
  };

  void AppendComment(userver::components::ComponentList &component_list);
}
