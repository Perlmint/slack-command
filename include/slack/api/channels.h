#pragma once

namespace slack
{
  namespace api
  {
#define ChannelsArchive_REQUIRED \
    API_FIELDS_DEF((std::string, channel))
#define ChannelsArchive_OPTIONAL BOOST_PP_LIST_NIL
#define ChannelsArchive_RESULT BOOST_PP_LIST_NIL

#define ChannelsCreate_REQUIRED \
    API_FIELDS_DEF((std::string, name))
#define ChannelsCreate_OPTIONAL BOOST_PP_LIST_NIL
#define ChannelsCreate_RESULT BOOST_PP_LIST_NIL

    API_NAMESPACE_BEGIN(Channels)
      API_ENDPOINT(ChannelsArchive, archive, "api/channels.archive");
      API_ENDPOINT(ChannelsCreate, create, "api/channels.create");
    API_NAMESPACE_END();
  }
}
