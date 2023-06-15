#include "comment.hpp"

namespace portfolio::comment {
  Comment::Comment(const userver::components::ComponentConfig &config,
          const userver::components::ComponentContext &context)
      : Base(config, context) {}

  userver::formats::json::Value Comment::HandleRequestJsonThrow(
      const userver::server::http::HttpRequest &request,
      const userver::formats::json::Value &request_json,
      userver::server::request::RequestContext &) const {
    std::string comment_id = request.GetPathArg("comment_id");

    if (comment_id.empty()) {
      switch (request.GetMethod()) {
        case userver::server::http::HttpMethod::kPost: {
          std::string error = CheckRequestData(request_json);
          if (!error.empty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            return utils::ResponseMessage(error);
          }
          return CreateObject(request, "comment", kInsertValue);
        }

        default:
          throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{
              fmt::format("Unsupported method {}", request.GetMethod())});
      }
    } else {
      switch (request.GetMethod()) {
        case userver::server::http::HttpMethod::kGet:
          return utils::ResponseMessage("Hello from GET comment by id");
        case userver::server::http::HttpMethod::kPatch: {
          std::string error = CheckRequestData(request_json);
          if (!error.empty()) {
            request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
            return utils::ResponseMessage(error);
          }
          // int id = stoi(utils::ParseRequestData(request_json, "comment_id"));
          return UpdateObject(request, "comment", kUpdateValue);
        }
        case userver::server::http::HttpMethod::kDelete:
          return DeleteObject(request, "comment", "comments", comment_id);

        default:
          throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{
              fmt::format("Unsupported method {}", request.GetMethod())});
      }
    }
  }

  std::string Comment::CheckRequestData(
      const userver::formats::json::Value &request_json) {
    std::string error;

    error = utils::ParseRequestData(
        request_json,
        "target_id", "target_type", "text"
    );
    if (!error.empty()) {
      return fmt::format("Field {} are required", error);
    }

    error = utils::ValidateRequestData("text", request_json["text"], 3);
    if (!error.empty()) {
      return error;
    }

    return error;
  }

  void AppendComment(userver::components::ComponentList &component_list) {
    component_list.Append<Comment>("handler-comment");
    component_list.Append<Comment>("handler-comment-id");
  }
} // namespace portfolio::comment
