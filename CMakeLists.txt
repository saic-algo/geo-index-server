cmake_minimum_required (VERSION 2.6)
project (S2Test)
add_executable(
  test-server
  geo-index-server-application.cc
  geo-index-request-handler.cc
  geo-index-request-handler-factory.cc
  s2-geo-index.cc
  redis-geo-index.cc
  h3-geo-index.cc
  request-handlers/base.cc
  request-handlers/performance-logger.cc
  request-handlers/create-index.cc
  request-handlers/delete-index.cc
  request-handlers/query-index.cc
)

target_link_libraries(test-server PocoNet PocoUtil PocoFoundation PocoJSON PocoRedis s2 h3)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
