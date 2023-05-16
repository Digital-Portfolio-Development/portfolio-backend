#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/auth/auth_checker_factory.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "services/comment_service/comment.hpp"
#include "services/project_service/project.hpp"
#include "services/user_service/auth_bearer.hpp"
#include "services/user_service/session.hpp"
#include "services/user_service/user.hpp"
#include "services/user_service/user_info_cache.hpp"

int main(int argc, char* argv[]) {
    // auth checker registration
    userver::server::handlers::auth::RegisterAuthCheckerFactory(
        "bearer", std::make_unique<portfolio::user::CheckerFactory>());

    // main
    auto component_list =
        userver::components::MinimalServerComponentList()
            .Append<portfolio::user::AuthCache>()
            .Append<userver::components::Secdist>()
            .Append<userver::components::DefaultSecdistProvider>()
            .Append<userver::server::handlers::Ping>()
            .Append<userver::components::TestsuiteSupport>()
            .Append<userver::components::HttpClient>()
            .Append<userver::components::Postgres>("portfolio-db")
            .Append<userver::server::handlers::TestsControl>()
            .Append<userver::clients::dns::Component>();

    // component list includes
    portfolio::user::AppendUser(component_list);
    portfolio::session::AppendSession(component_list);
    portfolio::project::AppendProject(component_list);
    portfolio::comment::AppendComment(component_list);

    return userver::utils::DaemonMain(argc, argv, component_list);
}
