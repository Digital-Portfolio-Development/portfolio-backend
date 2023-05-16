#include "project.hpp"

// TODO: реализовать похожую логику для сервиса поста
namespace portfolio::project {
Project::Project(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : Base(config, context) {}

userver::formats::json::Value Project::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext&) const {
    std::string key_word = request.GetArg("key");
    std::string project_id = request.GetPathArg("project_id");
    std::string comments_project_id = request.GetPathArg("comments_project_id");

    // Поиск проектов по тэгам
    if (!key_word.empty()) {
        return SearchAndGetObjects<utils::ProjectTuple>("project", "projects",
                                                        key_word);
    }

    /* TODO: разбить блоки if на разные функции:
     * 1) HandleProjectWithId
     * 2) HandleProjectComments
     * 3) HandleProject
     *
     */

    // TODO: сделать обработку update, delete методов

    // /api/project/{project_id}
    if (!project_id.empty()) {  // Обработка получения проекта по ID

        switch (request.GetMethod()) {  // [GET]
            case userver::server::http::HttpMethod::kGet: {
                request.SetResponseStatus(
                    userver::server::http::HttpStatus::kAccepted);
                return GetProjectByID(project_id);
            }

            default:
                throw userver::server::handlers::ClientError(
                    userver::server::handlers::ExternalBody{fmt::format(
                        "Unsupported method {}", request.GetMethod())});
        }

        // /api/project/comments/{comments_project_id}
    } else if (!comments_project_id
                    .empty()) {  // Обработка получения комментариев под
                                 // проектом по ID
        switch (request.GetMethod()) {
            case userver::server::http::HttpMethod::kGet: {
                request.SetResponseStatus(
                    userver::server::http::HttpStatus::kAccepted);
                return GetComments("project", comments_project_id);
            }
            default:
                throw userver::server::handlers::ClientError(
                    userver::server::handlers::ExternalBody{fmt::format(
                        "Unsupported method {}", request.GetMethod())});
        }
    } else {
        switch (request.GetMethod()) {  // [GET]
            case userver::server::http::HttpMethod::kPost: {
                std::string error = CheckRequestData(request_json);
                if (!error.empty()) {
                    request.SetResponseStatus(
                        userver::server::http::HttpStatus::kBadRequest);
                    return utils::ResponseMessage(error);
                }
                return CreateObject(request, "project", kInsertValue);
            }

            default:
                throw userver::server::handlers::ClientError(
                    userver::server::handlers::ExternalBody{fmt::format(
                        "Unsupported method {}", request.GetMethod())});
        }
    }

    return {};
}

std::string Project::CheckRequestData(
    const userver::formats::json::Value& request_json) {
    std::string error;

    error = utils::ParseRequestData(request_json, "name", "short_description",
                                    "full_description", "tags", "priority",
                                    "visibility");
    if (!error.empty()) return fmt::format("Field {} are required", error);

    std::map<std::string, int> setups = {
        {"name", 1},
        {"short_description", 1},
        {"full_description", 1},
    };

    for (const auto& x : setups) {
        error = utils::ValidateRequestData(x.first, request_json[x.first],
                                           x.second);
        if (!error.empty()) return error;
    }

    return error;
}

userver::formats::json::Value Project::GetProjectByID(
    std::string_view project_id) const {
    userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
        "get_project_transaction",
        userver::storages::postgres::ClusterHostType::kMaster, {});
    auto db_result = transaction.Execute(
        "SELECT p.*, u.username, u.user_avatar FROM projects p\n"
        "              JOIN users u ON p.user_id = u.user_id\n"
        "              WHERE (p.project_id = $1)",
        std::stoi(project_id.data()));

    auto result = db_result.AsSetOf<utils::ProjectTuple>(
        userver::storages::postgres::kRowTag);
    return utils::CreateJsonResult(result);
}

void AppendProject(userver::components::ComponentList& component_list) {
    component_list.Append<Project>("handler-project");
    component_list.Append<Project>("handler-project-id");
    component_list.Append<Project>("handler-project-comment");
}
}  // namespace portfolio::project
