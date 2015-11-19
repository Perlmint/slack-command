cmake_minimum_required(VERSION 2.6)

project(slack_command)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(compiler_option)

if (MSVC)
set(Boost_USE_STATIC_LIBS "On")
find_package(Boost 1.52 COMPONENTS date_time filesystem system thread regex REQUIRED)
else()
find_package(Boost 1.52 COMPONENTS date_time filesystem system thread REQUIRED)
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/crow
  ${Boost_INCLUDE_DIR}
  )

macro(crow_executable name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  SET_CPP_STD(MODE TARGET VERSION 14 ${name})
endmacro()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/example)
