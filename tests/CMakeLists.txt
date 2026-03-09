FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCES
    # Test files will be added as modules are implemented
)

set(CAPOLLO_CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/http_client.cpp
    ${CMAKE_SOURCE_DIR}/src/core/rest_client.cpp
    ${CMAKE_SOURCE_DIR}/src/core/auth.cpp
    ${CMAKE_SOURCE_DIR}/src/core/config.cpp
    ${CMAKE_SOURCE_DIR}/src/core/output.cpp
    ${CMAKE_SOURCE_DIR}/src/core/color.cpp
    ${CMAKE_SOURCE_DIR}/src/core/error.cpp
    ${CMAKE_SOURCE_DIR}/src/core/filter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/cache.cpp
    ${CMAKE_SOURCE_DIR}/src/core/resolver.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/contacts/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/contacts/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/accounts/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/accounts/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/deals/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/deals/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/tasks/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/tasks/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/sequences/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/sequences/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/enrichment/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/enrichment/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/stages/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/stages/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/labels/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/labels/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/users/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/users/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/fields/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/fields/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/email_accounts/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/email_accounts/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/emails/api.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/emails/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/config/commands.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/cache/commands.cpp
)

add_executable(apollo_tests
    ${TEST_SOURCES}
    ${CAPOLLO_CORE_SOURCES}
)

target_link_libraries(apollo_tests PRIVATE
    GTest::gtest_main
    nlohmann_json::nlohmann_json
    CLI11::CLI11
    tomlplusplus::tomlplusplus
    CURL::libcurl
    SQLite::SQLite3
)

target_include_directories(apollo_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_compile_options(apollo_tests PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow
)

include(GoogleTest)
gtest_discover_tests(apollo_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
