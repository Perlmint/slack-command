project(slack_command_examples)

include_directories(${CROW_INCLUDE_DIR})

crow_executable(echo_command ${CMAKE_CURRENT_SOURCE_DIR}/echo.cpp)
