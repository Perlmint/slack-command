#pragma once

namespace slack
{
  namespace api
  {
#define ApiTest_REQUIRED BOOST_PP_LIST_NIL
#define ApiTest_OPTIONAL \
    API_FIELDS_DEF((std::string, foo), \
                   (std::string, error))
#define ApiTest_RESULT \
    API_FIELDS_DEF((std::string, foo))

    API_NAMESPACE_BEGIN(Api)
      API_ENDPOINT(ApiTest, test, "api/api.test");
    API_NAMESPACE_END();
  }
}
