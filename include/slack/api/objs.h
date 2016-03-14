#pragma once
typedef std::map<std::string, std::string> str_map_t;

namespace slack
{
  template<typename T>
  void ParseResult(boost::optional<T> &out, const rapidjson::Value &val)
  {
    out = T(val);
  }

#define PARSER(TYPE) \
  template<> \
  void ParseResult<TYPE>(boost::optional<TYPE> &out, const rapidjson::Value &val)

  PARSER(std::string)
  {
    out = val.GetString();
  }

  PARSER(str_map_t)
  {
    str_map_t newVal;
    for (auto itr = val.MemberBegin(), end = val.MemberEnd(); itr != end; ++itr)
    {
      newVal.insert(std::make_pair(itr->name.GetString(), itr->value.GetString()));
    }
    out = newVal;
  }

  PARSER(int)
  {
    out = val.GetInt();
  }

  PARSER(unsigned int)
  {
    out = val.GetInt();
  }
  
  PARSER(float)
  {
    out = val.GetDouble();
  }
  
  PARSER(bool)
  {
    out = val.GetBool();
  }
}