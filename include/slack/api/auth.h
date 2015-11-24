#pragma once

namespace slack
{
  namespace api
  {
#define AuthTest_REQUIRED BOOST_PP_LIST_NIL
#define AuthTest_OPTIONAL BOOST_PP_LIST_NIL
#define AuthTest_RESULT \
    API_FIELDS_DEF( \
                   (std::string, url), \
                   (std::string, team), \
                   (std::string, user), \
                   (std::string, team_id), \
                   (std::string, user_id))

    API_NAMESPACE_BEGIN(Auth)
      API_ENDPOINT(AuthTest, test, "api/auth.test");
    API_NAMESPACE_END();
  }
}
