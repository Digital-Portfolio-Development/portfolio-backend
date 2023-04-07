#include <iostream>

#include <userver/utest/using_namespace_userver.hpp>
#include <userver/server/handlers/auth/auth_checker_factory.hpp>

namespace portfolio::user::auth {
  class CheckerFactory final : public userver::server::handlers::auth::AuthCheckerFactoryBase {
  public:
    userver::server::handlers::auth::AuthCheckerBasePtr operator()(
        const components::ComponentContext&,
        const server::handlers::auth::HandlerAuthConfig &auth_config,
        const server::handlers::auth::AuthCheckerSettings&) const override;
  };
} // namespace portfolio::user::auth
