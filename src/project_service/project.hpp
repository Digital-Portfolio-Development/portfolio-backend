#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <fmt/format.h>

#include <core/include/userver/components/component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace portfolio::project {
  struct ProjectStruct {
    std::int32_t project_id;
    std::string name;
    std::string description;
    std::string username;
    std::int32_t user_id;
    std::optional<std::string> avatar;
    std::int32_t comments_count;
    std::int32_t likes;
    std::int32_t views;
    userver::storages::postgres::TimePointTz created_at;
    std::optional<userver::storages::postgres::TimePointTz> updated_at;
  };

  std::string ValidateProject(const std::string &key, const std::string &value);

  void AppendProject(userver::components::ComponentList &component_list);
  void AppendProjectById(userver::components::ComponentList &component_list);
} // namespace portfolio::project