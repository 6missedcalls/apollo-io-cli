cmake_minimum_required(VERSION 3.20)
project(apollo VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

configure_file(src/core/version.h.in src/core/version.h)

include(FetchContent)

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_Declare(cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.4.2
)

FetchContent_Declare(tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(json cli11 tomlplusplus)

find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)

add_executable(apollo
    src/main.cpp
    src/core/http_client.cpp
    src/core/rest_client.cpp
    src/core/auth.cpp
    src/core/config.cpp
    src/core/output.cpp
    src/core/color.cpp
    src/core/error.cpp
    src/core/filter.cpp
    src/core/cache.cpp
    src/core/resolver.cpp
    src/modules/contacts/api.cpp
    src/modules/contacts/commands.cpp
    src/modules/accounts/api.cpp
    src/modules/accounts/commands.cpp
    src/modules/deals/api.cpp
    src/modules/deals/commands.cpp
    src/modules/tasks/api.cpp
    src/modules/tasks/commands.cpp
    src/modules/sequences/api.cpp
    src/modules/sequences/commands.cpp
    src/modules/enrichment/api.cpp
    src/modules/enrichment/commands.cpp
    src/modules/stages/api.cpp
    src/modules/stages/commands.cpp
    src/modules/labels/api.cpp
    src/modules/labels/commands.cpp
    src/modules/users/api.cpp
    src/modules/users/commands.cpp
    src/modules/fields/api.cpp
    src/modules/fields/commands.cpp
    src/modules/email_accounts/api.cpp
    src/modules/email_accounts/commands.cpp
    src/modules/emails/api.cpp
    src/modules/emails/commands.cpp
    src/modules/config/commands.cpp
    src/modules/cache/commands.cpp
)

target_include_directories(apollo PRIVATE src ${CMAKE_BINARY_DIR}/src)

target_link_libraries(apollo PRIVATE
    nlohmann_json::nlohmann_json
    CLI11::CLI11
    tomlplusplus::tomlplusplus
    CURL::libcurl
    SQLite::SQLite3
)

target_compile_options(apollo PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow
)

install(TARGETS apollo DESTINATION bin)

enable_testing()
add_subdirectory(tests)
