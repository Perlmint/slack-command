cmake_minimum_required(VERSION 2.6)

project(slacpp)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(compiler_option)

if (MSVC)
set(Boost_USE_STATIC_LIBS "On")
find_package(Boost 1.52 COMPONENTS date_time filesystem system thread regex REQUIRED)
else()
find_package(Boost 1.52 COMPONENTS date_time filesystem system thread REQUIRED)
endif()

find_package(OpenSSL 1)

set(CROW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/crow)
set(WEBSOCKETPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/websocketpp)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIR}
  )

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/example)
