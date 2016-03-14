#pragma once

namespace slack
{
  namespace api
  {
#define EmojiList_REQUIRED BOOST_PP_LIST_NIL
#define EmojiList_OPTIONAL BOOST_PP_LIST_NIL
#define EmojiList_RESULT \
    API_FIELDS_DEF((std::map<std::string, std::string>, emoji))

    API_NAMESPACE_BEGIN(Chat)
      API_ENDPOINT(EmojiList, archive, "api/emoji.list");
    API_NAMESPACE_END();
  }
}
