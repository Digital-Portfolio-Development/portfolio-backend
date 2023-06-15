#include "user.hpp"

namespace portfolio::user {
  User::User(const userver::components::ComponentConfig &config,
                   const userver::components::ComponentContext &context)
      : Base(config, context) {}

  userver::formats::json::Value User::HandleRequestJsonThrow(
      const userver::server::http::HttpRequest &request,
      const userver::formats::json::Value &request_json,
      userver::server::request::RequestContext &) const {

    switch (request.GetMethod()) {
      case userver::server::http::HttpMethod::kPost: {
        std::string error = CheckRequestData(request_json);
        if (!error.empty()) {
          request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
          return utils::ResponseMessage(error);
        }
        return CreateObject(request, "user", kInsertValue);
      }
      case userver::server::http::HttpMethod::kGet: {
        return
      }
      default:
        throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{
            fmt::format("Unsupported method {}", request.GetMethod())});
    }
  }

  std::string User::CheckRequestData(
      const userver::formats::json::Value &request_json) {
    std::string error;

    error = utils::ParseRequestData(
        request_json,
        "name", "last_name", "username", "password"
    );
    if (!error.empty()) return fmt::format("Field {} are required", error);

    std::map<std::string, int> setups = {
        {"name", 3},
        {"last_name", 3},
        {"username", 5},
        {"password", 8}
    };

    for (const auto &x : setups) {
      error =
          utils::ValidateRequestData(x.first, request_json[x.first], x.second);
      if (!error.empty()) return error;
    }

    return error;
  }

  void AppendUser(userver::components::ComponentList &component_list) {
    component_list.Append<User>();
  }
} // namespace portfolio::user
