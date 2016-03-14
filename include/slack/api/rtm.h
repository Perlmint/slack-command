#pragma once

namespace slack
{
  namespace api
  {
#define RtmStart_REQUIRED \
    API_FIELDS_DEF((std::string, token))
#define RtmStart_OPTIONAL \
    API_FIELDS_DEF( \
                   (bool, simple_latest), \
                   (bool, no_unreads), \
                   (bool, mpim_aware))
#define RtmStart_RESULT \
    API_FIELDS_DEF( \
                   (bool, ok), \
                   (std::string, url))

    API_NAMESPACE_BEGIN(Rtm)
      API_ENDPOINT(RtmStart, start, "api/rtm.start");
    API_NAMESPACE_END();
  }
}
