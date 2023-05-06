#include "utils.hpp"

namespace portfolio::utils {
userver::formats::json::Value ReturnValueAndSetStatus(
    const userver::server::http::HttpRequest &request,
    std::string message,
    userver::server::http::HttpStatus status){
  request.SetResponseStatus(status);
  return ResponseMessage(message);
}
} // namespace portfolio::utils