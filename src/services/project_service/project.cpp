#include "project.hpp"


// TODO: реализовать похожую логику для сервиса поста
namespace portfolio::project {
  Project::Project(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
        : Base(config, context) {}

  userver::formats::json::Value Project::HandleRequestJsonThrow(
      const userver::server::http::HttpRequest &request,
      const userver::formats::json::Value &request_json,
      userver::server::request::RequestContext &) const {
    std::string key_word = request.GetArg("key");
    std::string project_id = request.GetPathArg("project_id");
    std::string comments_project_id = request.GetPathArg("comments_project_id");

    if (!key_word.empty()) {
      return SearchAndGetObjects<utils::ProjectTuple>("project", "projects", key_word);
    }

    /* TODO: разбить блоки if на разные функции:
     * 1) HandleProjectWithId
     * 2) HandleProjectComments
     * 3) HandleProject
     *
     */
    if (!project_id.empty()) {

      switch (request.GetMethod()) {
        case userver::server::http::HttpMethod::kGet:
          return utils::ResponseMessage("Hi from GET project by id"); // TODO: сделать обработку update, delete методов

        default:
          throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{
              fmt::format("Unsupported method {}", request.GetMethod())});
      }
    } else if (!comments_project_id.empty()) {
      switch (request.GetMethod()) {
        case userver::server::http::HttpMethod::kGet: {
          std::string error = CheckRequestData(request_json);
          if (!error.empty()){
            request.SetResponseStatus(userver::server::http::HttpStatus::kAccepted);
            return GetComments("project", comments_project_id);
          }
        }
        default:
          throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{
              fmt::format("Unsupported method {}", request.GetMethod())});
      }
    } else {
      switch (request.GetMethod()) {
        case userver::server::http::HttpMethod::kPost: {
          std::string error = CheckRequestData(request_json);
          if (!error.empty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            return utils::ResponseMessage(error);
          }
          return CreateObject(request, "project", kInsertValue);
        }

        default:
          throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{
              fmt::format("Unsupported method {}", request.GetMethod())});
      }
    }

    return {};
  }

  std::string Project::CheckRequestData(
      const userver::formats::json::Value &request_json) {
    std::string error;

    error = utils::ParseRequestData(
        request_json,
        "name", "short_description", "full_description", "tags", "priority", "visibility"
    );
    if (!error.empty()) return fmt::format("Field {} are required", error);

    std::map<std::string, int> setups = {
        {"name", 1},
        {"short_description", 1},
        {"full_description", 1},
    };

    for (const auto &x : setups) {
      error = utils::ValidateRequestData(x.first, request_json[x.first], x.second);
      if (!error.empty()) return error;
    }

    return error;
  }

  void AppendProject(userver::components::ComponentList &component_list) {
    component_list.Append<Project>("handler-project");
    component_list.Append<Project>("handler-project-id");
    component_list.Append<Project>("handler-project-comment");
  }
} // namespace portfolio::project
