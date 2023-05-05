#include "utils.hpp"

namespace portfolio::utils {
  std::string ValidateRequestData(
      const std::string &key,
      userver::formats::json::Value &&value_json,
      std::size_t len,
      bool is_email) {
    std::string value = value_json.As<std::string>();
    std::string error;

    if (is_email) {
      userver::utils::regex pattern = userver::utils::regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
      if (!userver::utils::regex_match(value, pattern)) {
        error = fmt::format("{} is invalid", key);
      }
    } else {
      if (value.length() < len) {
        error = fmt::format("{} is too short", key);
      }
    }

    return error;
  }
} // namespace portfolio::utils