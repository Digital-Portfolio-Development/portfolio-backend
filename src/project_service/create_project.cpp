#include "create_project.hpp"

#include <fmt/format.h>

#include <userver/server/handlers/http_handler_base.hpp>

namespace portfolio::project {
  namespace {
    class CreateProject final : public userver::server::handlers::HttpHandlerBase {
    public:
      static constexpr std::string_view kName = "handler-project-create";

      using HttpHandlerBase::HttpHandlerBase;

      std::string HandleRequestThrow(
          const userver::server::http::HttpRequest &request,
          userver::server::request::RequestContext &) const override {
        std::string arg = request.GetArg("name");

        return portfolio::project::CreateProjectHandler(arg);
      }
    };
  } // namespace

  std::string CreateProjectHandler(std::string_view name) {
      return fmt::format("Project {} was created!", name);
  }

  void AppendCreateProject(userver::components::ComponentList &component_list) {
      component_list.Append<CreateProject>();
  }
} // namespace portfolio::project
