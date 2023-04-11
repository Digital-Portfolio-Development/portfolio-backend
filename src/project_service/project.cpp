#include "project.hpp"

#include <fmt/format.h>

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_response_cookie.hpp>
#include <userver/server/request/response_base.hpp>
#include <userver/formats/serialize/common_containers.hpp>


namespace portfolio::project {
  class Project final : public userver::server::handlers::HttpHandlerBase {
  public:
    static constexpr std::string_view kName = "handler-project";

    Project(const userver::components::ComponentConfig &config,
               const userver::components::ComponentContext &context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest &request,
        userver::server::request::RequestContext &) const override;

  private:
    std::string CreateProject(const userver::server::http::HttpRequest &request) const;
    std::string GetProjects() const;

    userver::storages::postgres::ClusterPtr pg_cluster_;
  };
} // namespace portfolio::project

namespace portfolio::project {
  Project::Project(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
        : HttpHandlerBase(config, context), pg_cluster_(context
                      .FindComponent<userver::components::Postgres>("portfolio-db")
                      .GetCluster()){}

  std::string Project::HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const {

    switch (request.GetMethod()) {
      case userver::server::http::HttpMethod::kGet:
        return GetProjects();
      case userver::server::http::HttpMethod::kPost:
        return CreateProject(request);
      default:
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{
          fmt::format("Unsupported method {}", request.GetMethod())});
    }
  }

  const userver::storages::postgres::Query kInsertValue {
      "INSERT INTO projects (name, description, username, user_id)"
      "VALUES ($1, $2, $3, $4)",
      userver::storages::postgres::Query::Name {"project_insert_value"}
  };

  std::string Project::CreateProject(
      const userver::server::http::HttpRequest &request) const {
    std::string user_id = request.GetCookie("user_id");
    if (user_id.empty()) {
      request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
      return "Unauthorized";
    }

    std::string error;

    userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
        "create_project_transaction",
        userver::storages::postgres::ClusterHostType::kMaster,
        {}
    );

    userver::storages::postgres::ResultSet candidate =
        transaction.Execute("SELECT username from users WHERE user_id = $1", std::stoi(user_id));

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
    std::string username = candidate[0]["username"].As<std::string>();

    auto res = transaction.Execute(kInsertValue, name, description, username, std::stoi(user_id));

    if (res.RowsAffected()) {
      transaction.Commit();
      request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
      return fmt::format("Project {} was created!", name);
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
    return "Unknown error";
  }

  std::string Project::GetProjects() const {
    auto transaction = pg_cluster_->Begin(
        "get_projects_transaction",
        userver::storages::postgres::ClusterHostType::kMaster,
        {}
    );

    auto projects = transaction.Execute("SELECT * FROM projects");
    auto iteration = projects.AsSetOf<ProjectStruct>(userver::storages::postgres::kRowTag);

    userver::formats::json::ValueBuilder json_res;

    for (auto row : iteration) {
      json_res[fmt::format("project {}", row.project_id)]["project_id"] = row.project_id;
      json_res[fmt::format("project {}", row.project_id)]["name"] = row.name;
      json_res[fmt::format("project {}", row.project_id)]["description"] = row.description;
      json_res[fmt::format("project {}", row.project_id)]["username"] = row.username;
      json_res[fmt::format("project {}", row.project_id)]["user_id"] = row.user_id;
      json_res[fmt::format("project {}", row.project_id)]["avatar"] = row.avatar;
      json_res[fmt::format("project {}", row.project_id)]["comments_count"] = row.comments_count;
      json_res[fmt::format("project {}", row.project_id)]["likes"] = row.likes;
      json_res[fmt::format("project {}", row.project_id)]["views"] = row.views;
      json_res[fmt::format("project {}", row.project_id)]["created_at"] = row.created_at;
      json_res[fmt::format("project {}", row.project_id)]["updated_at"] = row.updated_at;
    }

    return userver::formats::json::ToString(json_res.ExtractValue());
  }

  std::string ProjectValidation(const std::string &key, const std::string &value) {
    std::string error;

    if (key == "name") {
      if (!value.length()) {
        error = "Field name can't be empty";
      }
    } else if (key == "description") {
      if (!value.length()) {
        error = "Field description can't be empty";
      }
    }

    return error;
  }

  void AppendProject(userver::components::ComponentList &component_list) {
    component_list.Append<Project>();
  }
} // namespace portfolio::project
