#include <iostream>

#include <core/include/userver/server/handlers/auth/auth_checker_factory.hpp>
#include <shared/include/userver/utest/using_namespace_userver.hpp>

namespace portfolio::user {
  class CheckerFactory final : public userver::server::handlers::auth::AuthCheckerFactoryBase {
  public:
    userver::server::handlers::auth::AuthCheckerBasePtr operator()(
        const components::ComponentContext&,
        const server::handlers::auth::HandlerAuthConfig &auth_config,
        const server::handlers::auth::AuthCheckerSettings&) const override;
  };
} // namespace portfolio::user
