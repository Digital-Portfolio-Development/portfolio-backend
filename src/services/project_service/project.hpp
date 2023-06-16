#include "../../base.hpp"

namespace portfolio::project {
class Project final : public base::Base {
   public:
    static constexpr std::string_view kName = "handler-project";

    Project(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context);

    // Data that doesn't come in the request body (like user_id)
    // MUST be at the beginning of the field list.
    // Data that can be NULL (like media)
    // MUST be at the end of the field list and
    // MUST be marked as DEFAULT in the value list.
    const userver::storages::postgres::Query kInsertValue{
        "INSERT INTO projects "
        "(user_id, name, short_description, full_description, tags, priority, "
        "visibility)"
        "VALUES ($1, $2, $3, $4, $5, $6, $7)",
        userver::storages::postgres::Query::Name{"project_insert_value"}};

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext&) const override;

    static std::string CheckRequestData(
        const userver::formats::json::Value& request_json);

    userver::formats::json::Value GetProjectByID(std::string_view id) const;
};

void AppendProject(userver::components::ComponentList& component_list);
}  // namespace portfolio::project