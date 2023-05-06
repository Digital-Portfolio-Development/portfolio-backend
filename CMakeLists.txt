cmake_minimum_required(VERSION 3.12)
project(portfolio CXX)

include(third_party/userver/cmake/SetupEnvironment.cmake)
include(GNUInstallDirs)

add_subdirectory(third_party/userver)
add_subdirectory(third_party/jwt-cpp)

set(CMAKE_CXX_STANDARD 20)

# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
	src/base.hpp
	src/services/user_service/user.hpp
	src/services/user_service/session.hpp
	src/services/user_service/auth_bearer.hpp
	src/services/user_service/user_info_cache.hpp
	src/services/project_service/project.hpp
	src/services/comment_service/comment.hpp
	src/utils/utils.hpp
)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver-core userver-postgresql)

include_directories(SYSTEM ./third_party/userver)
include_directories(SYSTEM ./third_party/jwt-cpp/include)

# The Service
add_executable(${PROJECT_NAME}
	src/main.cpp
	src/base.cpp
	src/base.hpp
	src/services/user_service/user.cpp
	src/services/user_service/user.hpp
	src/services/user_service/session.cpp
	src/services/user_service/session.hpp
	src/services/user_service/auth_bearer.cpp
	src/services/user_service/auth_bearer.hpp
	src/services/user_service/user_info_cache.hpp
	src/services/project_service/project.cpp
	src/services/project_service/project.hpp
	src/services/comment_service/comment.cpp
	src/services/comment_service/comment.hpp
	src/utils/validate_request.cpp
	src/utils/response_message.cpp
        src/utils/ReturnValueAndSetStatus.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)

# Functional Tests
add_subdirectory(tests)

if(DEFINED ENV{PREFIX})
	message(STATUS "Set install prefix: $ENV{PREFIX}")
	file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
	set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

set(CONFIG_VAR_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}/config_vars.yaml)
set(CONFIG_FALLBACK_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}/dynamic_config_fallback.json)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configs/static_config.yaml.in ${CMAKE_CURRENT_SOURCE_DIR}/configs/static_config.yaml)

FILE(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
