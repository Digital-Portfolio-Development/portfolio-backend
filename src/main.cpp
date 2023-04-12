#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/storages/postgres/component.hpp>

#include "user_service/auth_bearer.hpp"
#include "user_service/create_session.hpp"
#include "user_service/create_user.hpp"
#include "user_service/user_info_cache.hpp"
#include "project_service/create_project.hpp"

int main(int argc, char* argv[]) {
  // auth checker registration
  userver::server::handlers::auth::RegisterAuthCheckerFactory(
      "bearer", std::make_unique<portfolio::user::CheckerFactory>());

  // main
  auto component_list = userver::components::MinimalServerComponentList()
                            .Append<portfolio::user::AuthCache>()
                            .Append<userver::server::handlers::Ping>()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::components::Postgres>("portfolio-db")
                            .Append<userver::components::HttpClient>()
                            .Append<userver::server::handlers::TestsControl>();

  // component list
  portfolio::user::AppendRegisterUser(component_list);
  portfolio::user::AppendLoginUser(component_list);
  portfolio::project::AppendCreateProject(component_list);

  return userver::utils::DaemonMain(argc, argv, component_list);
}
