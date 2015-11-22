project(slack_command_examples)

include_directories(${CROW_INCLUDE_DIR})

macro(crow_executable name)
  add_executable(${name} ${ARGN})
  target_link_libraries(${name} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  SET_CPP_STD(MODE TARGET VERSION 14 ${name})
endmacro()

crow_executable(echo_command ${CMAKE_CURRENT_SOURCE_DIR}/echo.cpp)
