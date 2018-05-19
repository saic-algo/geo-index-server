cmake_minimum_required (VERSION 2.6)
project (S2Test)
add_executable(
  test-server
  geo-index-server-application.cc
  geo-index-request-handler.cc
  geo-index-request-handler-factory.cc
  request-handlers/create-index.cc
  request-handlers/delete-index.cc
  request-handlers/query-index.cc
  request-handlers/create-index-redis.cc
  request-handlers/delete-index-redis.cc
  request-handlers/query-index-redis.cc
)
target_link_libraries(test-server PocoNet PocoUtil PocoFoundation PocoJSON s2)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
