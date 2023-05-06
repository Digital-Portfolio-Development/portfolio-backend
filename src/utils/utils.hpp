#include <string>
#include <vector>

#include <userver/formats/json.hpp>
#include <userver/utils/regex.hpp>
#include <userver/server/http/http_request.hpp>

namespace portfolio::utils {
  template<class ...T>
  std::string ParseRequestData(
      const userver::formats::json::Value &request, T&& ...args) {
    std::vector<std::string> fields;
    std::string required_fields;

    for (auto [key, _] : userver::formats::json::Items(request)) {
      fields.push_back(key);
    }

    for (const auto x : {args...}) {
      if (std::find(fields.begin(), fields.end(), x) == fields.end()) {
        auto separator = (required_fields.empty() ? "" : ",");
        required_fields += fmt::format("{}{}", separator, x);
      }
    }

    return required_fields;
  }

  std::string ValidateRequestData(
      const std::string &key,
      userver::formats::json::Value &&value_json,
      std::size_t len,
      bool is_email = false);

  userver::formats::json::Value ResponseMessage(
      const std::string &message);

  userver::formats::json::Value ReturnValueAndSetStatus(
      const userver::server::http::HttpRequest &request,
      std::string message,
      userver::server::http::HttpStatus status);

}
