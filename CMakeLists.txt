cmake_minimum_required(VERSION 2.6)

project(slacpp)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(compiler_option)

option(EXT_HEADERS_FROM_SOURCE
  "Use external headers(crow, websocketpp) from full source tree"
  OFF)

if(MSVC)
  set(Boost_USE_STATIC_LIBS "On")
  find_package(Boost 1.52 COMPONENTS date_time filesystem system thread regex REQUIRED)
else()
  find_package(Boost 1.52 COMPONENTS date_time filesystem system thread REQUIRED)
endif()

find_package(OpenSSL 1)

if(EXT_HEADERS_FROM_SOURCE)
  set(CROW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/crow/repo/include)
  set(WEBSOCKETPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/websocketpp/repo)
  set(RAPIDJSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/repo/include)
else()
  set(CROW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/crow)
  set(WEBSOCKETPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/websocketpp)
  set(RAPIDJSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson)
endif()

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(
  ${INC_DIR}
  ${Boost_INCLUDE_DIR}
  )

set(SLACPP_HDRS
  ${INC_DIR}/slack/api.h
  ${INC_DIR}/slack/command.h
  ${INC_DIR}/slack/api/base.h
  ${INC_DIR}/slack/api/namespaces.h
  ${INC_DIR}/slack/api/api.h
  ${INC_DIR}/slack/api/auth.h
  ${INC_DIR}/slack/api/chat.h
  ${INC_DIR}/slack/api/emoji.h
  ${INC_DIR}/slack/api/error.h
  ${INC_DIR}/slack/api/rtm.h)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/example)

set(GROUP_PREFIX "Header Files\\slack")
source_group("${GROUP_PREFIX}" REGULAR_EXPRESSION "slack/[^/]+.h")
source_group("${GROUP_PREFIX}\\api" REGULAR_EXPRESSION "slack/api/[^/]+.h")
