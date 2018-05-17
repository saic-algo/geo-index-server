cmake_minimum_required (VERSION 2.6)
project (S2Test)
add_executable(
  test-server
  geo-index-server-application.cc
  geo-index-request-handler.cc
  geo-index-request-handler-factory.cc
  request-handlers/create-index.cc
)
target_link_libraries(test-server PocoNet PocoUtil PocoFoundation PocoJSON s2)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
