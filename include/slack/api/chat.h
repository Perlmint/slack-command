#pragma once

namespace slack
{
  namespace api
  {
#define ChatDelete_REQUIRED \
    API_FIELDS_DEF((std::string, ts), (std::string, channel))
#define ChatDelete_OPTIONAL BOOST_PP_LIST_NIL
#define ChatDelete_RESULT \
    API_FIELDS_DEF((std::string, ts), (std::string, channel))

    API_NAMESPACE_BEGIN(Chat)
      API_ENDPOINT(ChatDelete, archive, "api/chat.delete");
    API_NAMESPACE_END();
  }
}
