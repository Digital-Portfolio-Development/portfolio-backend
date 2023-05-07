#include <string>
#include <vector>

#include <userver/storages/postgres/result_set.hpp>
#include <userver/formats/json.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/utils/regex.hpp>

namespace portfolio::utils {
  using UserTuple = std::tuple<
      int, std::string, std::string, std::string,
      std::optional<std::string>, std::optional<std::string>, std::string,
      std::optional<std::string>, std::optional<std::string>,
      std::optional<std::string>, std::optional<std::string>,
      std::optional<std::string>, std::optional<std::string>, std::optional<std::string>,
      userver::storages::postgres::TimePointTz,
      std::optional<userver::storages::postgres::TimePointTz>
  >;
  using ProjectTuple = std::tuple<
      int, int, std::string, std::string, std::string,
      std::string, std::optional<std::string>, std::optional<std::string>,
      int, bool, int, userver::storages::postgres::TimePointTz,
      std::optional<userver::storages::postgres::TimePointTz>,
      std::string, std::optional<std::string>
  >;
  using PostTuple = std::tuple<
      int, int, std::string, std::string, std::optional<std::string>,
      std::string, userver::storages::postgres::TimePointTz,
      std::optional<userver::storages::postgres::TimePointTz>,
      std::string, std::optional<std::string>
  >;
  using CommentTuple = std::tuple<
      int, int, int, std::string, std::string, std::optional<std::string>,
      userver::storages::postgres::TimePointTz,
      std::optional<userver::storages::postgres::TimePointTz>,
      std::string, std::optional<std::string>
  >;
  using LikeTuple = std::tuple<
      int, int, std::string, std::string,
      userver::storages::postgres::TimePointTz
  >;

  userver::formats::json::Value CreateJsonResult(
      userver::storages::postgres::TypedResultSet<
          UserTuple, userver::storages::postgres::RowTag> &iteration);
  userver::formats::json::Value CreateJsonResult(
      userver::storages::postgres::TypedResultSet<
          ProjectTuple, userver::storages::postgres::RowTag> &iteration);
  userver::formats::json::Value CreateJsonResult(
      userver::storages::postgres::TypedResultSet<
          PostTuple, userver::storages::postgres::RowTag> &iteration);
  userver::formats::json::Value CreateJsonResult(
      userver::storages::postgres::TypedResultSet<
          CommentTuple, userver::storages::postgres::RowTag> &iteration);
  userver::formats::json::Value CreateJsonResult(
      userver::storages::postgres::TypedResultSet<
          LikeTuple, userver::storages::postgres::RowTag> &iteration);

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
}
