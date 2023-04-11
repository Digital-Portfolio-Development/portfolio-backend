#include <string>
#include <string_view>

#include <core/include/userver/components/component_list.hpp>

namespace portfolio::project {
  std::string CreateProjectHandler(std::string_view name);
  void AppendCreateProject(userver::components::ComponentList &component_list);
} // namespace portfolio::project