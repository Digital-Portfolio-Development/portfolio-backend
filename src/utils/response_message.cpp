#include "utils.hpp"

namespace portfolio::utils {
  userver::formats::json::Value ResponseMessage(
      const std::string &message) {
    userver::formats::json::ValueBuilder response;
    response["message"] = message;

    return response.ExtractValue();
  }
} // namespace portfolio::utils