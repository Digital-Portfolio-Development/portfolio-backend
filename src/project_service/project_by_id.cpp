#include "project.hpp"

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/formats/serialize/common_containers.hpp>


namespace portfolio::project {
  class ProjectById final : public userver::server::handlers::HttpHandlerBase {
  public:
    static constexpr std::string_view kName = "handler-project-id";

    ProjectById(const userver::components::ComponentConfig &config,
                  const userver::components::ComponentContext &context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest &request,
        userver::server::request::RequestContext &) const override;

  private:
    std::string GetOneProject(const userver::server::http::HttpRequest &request) const;
    std::string UpdateOneProject(const userver::server::http::HttpRequest &request) const;
    std::string DeleteOneProject(const userver::server::http::HttpRequest &request) const;

    userver::storages::postgres::ClusterPtr pg_cluster_;
  };
} // namespace portfolio::project

namespace portfolio::project {
  ProjectById::ProjectById(const userver::components::ComponentConfig &config,
            const userver::components::ComponentContext &context)
    : HttpHandlerBase(config, context), pg_cluster_(context
                      .FindComponent<userver::components::Postgres>("portfolio-db")
                      .GetCluster()) {}

  std::string ProjectById::HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const {

    switch (request.GetMethod()) {
      case userver::server::http::HttpMethod::kGet:
        return GetOneProject(request);
      case userver::server::http::HttpMethod::kPatch:
        return UpdateOneProject(request);
      case userver::server::http::HttpMethod::kDelete:
        return DeleteOneProject(request);
      default:
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{
          fmt::format("Unsupported method {}", request.GetMethod())});
    }
  }

  std::string ProjectById::GetOneProject(
      const userver::server::http::HttpRequest &request) const {
    std::string project_id = request.GetPathArg("project_id");

    auto transaction = pg_cluster_->Begin(
        "get_project_transaction",
        userver::storages::postgres::ClusterHostType::kMaster,
        {}
    );

    transaction.Execute(
        "UPDATE projects SET views = views + 1 WHERE project_id = $1 RETURNING *",
        std::stoi(project_id)
    );

    auto project = transaction.Execute(
        "SELECT * FROM projects WHERE project_id = $1",
        std::stoi(project_id)
    );
    if (project.IsEmpty()) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
      return "Bad request";
    }
    transaction.Commit();

    auto iteration =
        project.AsSetOf<ProjectStruct>(userver::storages::postgres::kRowTag);

    userver::formats::json::ValueBuilder json_res;

    for (auto row : iteration) {
      json_res["project_id"] = row.project_id;
      json_res["name"] = row.name;
      json_res["description"] = row.description;
      json_res["username"] = row.username;
      json_res["user_id"] = row.user_id;
      json_res["avatar"] = row.avatar;
      json_res["comments_count"] = row.comments_count;
      json_res["likes"] = row.likes;
      json_res["views"] = row.views;
      json_res["created_at"] = row.created_at;
      json_res["updated_at"] = row.updated_at;
    }

    return userver::formats::json::ToString(json_res.ExtractValue());
  }

  const userver::storages::postgres::Query kUpdateValue {
      "UPDATE projects "
      "SET name = $1, description = $2, user_id = $3"
      "WHERE project_id = $4",
      userver::storages::postgres::Query::Name {"project_update_value"}
  };

  std::string ProjectById::UpdateOneProject(
      const userver::server::http::HttpRequest& request) const {
    std::string user_id = request.GetCookie("user_id");
    if (user_id.empty()) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
      return "Unauthorized";
    }

    std::string project_id = request.GetPathArg("project_id");
    std::string error;

    auto transaction = pg_cluster_->Begin(
        "update_project_transaction",
        userver::storages::postgres::ClusterHostType::kMaster,
        {}
    );

    userver::formats::json::Value body = userver::formats::json::FromString(request.RequestBody());

    // Validation
    for (auto [key, value] : userver::formats::json::Items(body)) {
      error = portfolio::project::ProjectValidation(key, value.ConvertTo<std::string>());
      if (!error.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return error;
      }
    }

    std::string name = body["name"].As<std::string>();
    std::string description = body["description"].As<std::string>();

    auto res = transaction.Execute(kUpdateValue, name, description, std::stoi(user_id), std::stoi(project_id));

    if (res.RowsAffected()) {
      transaction.Commit();
      request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
      return fmt::format("Project {} was updated!", name);
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return "Project with this id doesn't exist";
  }

  std::string ProjectById::DeleteOneProject(
      const userver::server::http::HttpRequest& request) const {
    std::string user_id = request.GetCookie("user_id");
    if (user_id.empty()) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
      return "Unauthorized";
    }
    std::string project_id = request.GetPathArg("project_id");

    auto transaction = pg_cluster_->Begin(
        "update_project_transaction",
        userver::storages::postgres::ClusterHostType::kMaster,
        {}
    );

    transaction.Execute(
        "DELETE FROM projects WHERE project_id = $1 AND user_id = $2",
        std::stoi(project_id), std::stoi(user_id)
    );
    transaction.Commit();

    request.SetResponseStatus(userver::server::http::HttpStatus::kResetContent);
    return fmt::format("Project was deleted!");
  }

  void AppendProjectById(userver::components::ComponentList &component_list) {
    component_list.Append<ProjectById>();
  }
} // namespace portfolio::project
