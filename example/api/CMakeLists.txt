project(slack_api_examples)

include_directories(${WEBSOCKETPP_INCLUDE_DIR} ${RAPIDJSON_INCLUDE_DIR})

macro(api_executable name)
  add_executable(${name} ${SLACPP_HDRS} ${ARGN})
  target_link_libraries(${name} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES})
  SET_CPP_STD(MODE TARGET VERSION 14 ${name})
endmacro()

api_executable(Test test.cpp)
api_executable(MessageEcho message_echo.cpp)
