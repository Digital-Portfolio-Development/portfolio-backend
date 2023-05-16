#include "session.hpp"
#include "../../utils/utils.hpp"

namespace portfolio::session {
Session::Session(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context)
    : HttpHandlerJsonBase(config, context),
      pg_cluster_(
          context.FindComponent<userver::components::Postgres>("portfolio-db")
              .GetCluster()) {}

userver::formats::json::Value Session::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext&) const {
    std::string error = CheckRequestData(request_json);
    if (!error.empty()) {
        request.SetResponseStatus(
            userver::server::http::HttpStatus::kBadRequest);
        return utils::ResponseMessage(error);
    }

    userver::storages::postgres::Transaction transaction = pg_cluster_->Begin(
        "login_transaction",
        userver::storages::postgres::ClusterHostType::kMaster, {});

    std::string username = request_json["username"].As<std::string>();
    std::string password = request_json["password"].As<std::string>();

    userver::storages::postgres::ResultSet candidate = transaction.Execute(
        "SELECT * from users WHERE username = $1", username);

    if (candidate.IsEmpty() || userver::crypto::hash::Sha256(password) !=
                                   candidate[0]["password"].As<std::string>()) {
        request.SetResponseStatus(
            userver::server::http::HttpStatus::kBadRequest);
        return utils::ResponseMessage("Invalid email or password");
    }

    int user_id = candidate[0]["user_id"].As<int>();
    std::string token = jwt::create()
                            .set_issuer("auth0")
                            .set_type("JWS")
                            .set_payload_claim("token", jwt::claim(username))
                            .sign(jwt::algorithm::hs256{"secret-key"});

    userver::server::http::Cookie cookie1("user_id", std::to_string(user_id));
    userver::server::http::Cookie cookie2("access_key", token);
    cookie1.SetHttpOnly();
    cookie1.SameSite();
    cookie1.SetPath("/");
    cookie2.SetHttpOnly();
    cookie2.SameSite();
    cookie2.SetPath("/");

    // work only with SSL/HTTPS
    // cookie1.SetSecure();
    // cookie2.SetSecure();

    request.GetHttpResponse().SetCookie(cookie1);
    request.GetHttpResponse().SetCookie(cookie2);

    return utils::ResponseMessage("Success auth!");
}

std::string Session::CheckRequestData(
    const userver::formats::json::Value& request_json) {
    std::string error;

    error = utils::ParseRequestData(request_json, "username", "password");
    if (!error.empty()) {
        return fmt::format("Field {} are required", error);
    }

    std::map<std::string, int> setups = {{"username", 5}, {"password", 8}};

    for (const auto& x : setups) {
        error = utils::ValidateRequestData(x.first, request_json[x.first],
                                           x.second);
        if (!error.empty()) {
            return error;
        }
    }

    return error;
}

void AppendSession(userver::components::ComponentList& component_list) {
    component_list.Append<Session>();
}
}  // namespace portfolio::session
