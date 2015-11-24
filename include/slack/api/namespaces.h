#pragma once

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/list/adt.hpp>
#include <boost/preprocessor/list/size.hpp>
#include <boost/preprocessor/list/for_each.hpp>
#include <boost/preprocessor/list/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_list.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include "base.h"

#define API_FIELDS_DEF(...) \
  BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)

#define COMMA_IF_LIST(list) \
  BOOST_PP_COMMA_IF(BOOST_PP_LIST_SIZE(list))

#define API_FIELD(r, data, item) \
  private: \
    boost::optional<BOOST_PP_TUPLE_ELEM(2, 0, item)> BOOST_PP_CAT(_, BOOST_PP_TUPLE_ELEM(2, 1, item)); \
  public: \
    const boost::optional<BOOST_PP_TUPLE_ELEM(2, 0, item)> &BOOST_PP_TUPLE_ELEM(2, 1, item)() const \
    { \
      return BOOST_PP_CAT(_, BOOST_PP_TUPLE_ELEM(2, 1, item)); \
    } \
    auto &BOOST_PP_TUPLE_ELEM(2, 1, item)(const BOOST_PP_TUPLE_ELEM(2, 0, item) &val) \
    { \
      BOOST_PP_CAT(_, BOOST_PP_TUPLE_ELEM(2, 1, item)) = val; \
      return *this; \
    }

#define API_REQUIRED_FIELD(r, data, item) \
  private: \
    BOOST_PP_TUPLE_ELEM(2, 0, item) BOOST_PP_CAT(_, BOOST_PP_TUPLE_ELEM(2, 1, item)); \
  public: \
    const BOOST_PP_TUPLE_ELEM(2, 0, item) &BOOST_PP_TUPLE_ELEM(2, 1, item)() const \
    { \
      return BOOST_PP_CAT(_, BOOST_PP_TUPLE_ELEM(2, 1, item));  \
    }

    class ResultBase
    {
    public:
      API_FIELD(0, 0, (bool, ok));
    };

#define API_FIELDS_EXPAND(list, func) \
  BOOST_PP_LIST_FOR_EACH(func, 0, list)

#define API_FIELD_TO_ARG_DEF(r, data, i, elem) \
  BOOST_PP_COMMA_IF(i) const BOOST_PP_TUPLE_ELEM(2, 0, elem) & BOOST_PP_CAT(__, BOOST_PP_TUPLE_ELEM(2, 1, elem))

#define API_FIELD_TO_ARG_DEFS(list) BOOST_PP_LIST_FOR_EACH_I(API_FIELD_TO_ARG_DEF, 0, list)

#define API_FIELD_TO_ARG(r, data, i, elem) \
  BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(__, BOOST_PP_TUPLE_ELEM(2, 1, elem))

#define API_FIELD_TO_ARGS(list) BOOST_PP_LIST_FOR_EACH_I(API_FIELD_TO_ARG, 0, list)

#define API_FIELD_TO_INIT(r, data, i, elem) \
  BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(_, BOOST_PP_TUPLE_ELEM(2, 1, elem)) (BOOST_PP_CAT(__, BOOST_PP_TUPLE_ELEM(2, 1, elem)))

#define API_FIELD_TO_INITS(list) BOOST_PP_LIST_FOR_EACH_I(API_FIELD_TO_INIT, 0, list)

#define API_OBJECT_RESULT_DECL(name) \
    public: \
      struct Result : ResultBase \
      { \
        API_FIELDS_EXPAND(name ## _RESULT, API_FIELD) \
        Result(const std::string &raw) \
        { \
          /* TODO: implement parse */ \
        } \
      }

#define API_OBJECT_DECL(name, url)\
    class name : public ApiBase \
    { \
    public: \
      name(Client *client COMMA_IF_LIST(name ## _REQUIRED) \
           API_FIELD_TO_ARG_DEFS(name ## _REQUIRED)) \
        : ApiBase(client) COMMA_IF_LIST(name ##_REQUIRED) \
          API_FIELD_TO_INITS(name ## _REQUIRED) {} \
    private: \
      static constexpr const char * const request_url() { return #url; } \
      using this_t = name; \
    API_OBJECT_RESULT_DECL(name); \
    API_FIELDS_EXPAND(name ## _REQUIRED, API_REQUIRED_FIELD) \
    API_FIELDS_EXPAND(name ## _OPTIONAL, API_FIELD) \
    public: \
      std::future<Result> operator()() \
      { \
        std::shared_ptr<std::promise<Result>> promise(new std::promise<Result>()); \
        std::future<Result> ret = promise->get_future(); \
        std::function<void(Result &)> promiseCallback = std::bind(&this_t::promise_callback, promise, std::placeholders::_1); \
        run(request_url(), std::bind(&this_t::parse, std::placeholders::_1, promiseCallback)); \
        return ret; \
      } \
\
      void operator()(const std::function<void(Result &)> &callback) \
      { \
        run(request_url(), std::bind(&this_t::parse, std::placeholders::_1, callback)); \
      } \
    private: \
      static void promise_callback(std::shared_ptr<std::promise<Result> > promise, Result result) \
      { \
        promise->set_value(result); \
      } \
\
      static void parse(const std::string &raw, const std::function<void(Result &)> &callback) \
      { \
        Result result(raw); \
        callback(result); \
      } \
    }

#define API_NAMESPACE_BEGIN(name) \
    class name : public ApiBase \
    { \
    public: \
      name(Client *client) \
        : ApiBase(client) {}

#define API_NAMESPACE_END(); }

#define API_ENDPOINT(type, name, url) \
    API_OBJECT_DECL(type, url); \
    type name(API_FIELD_TO_ARG_DEFS(type ## _REQUIRED)) \
    { \
      return type{client COMMA_IF_LIST(type ## _REQUIRED) API_FIELD_TO_ARGS(type ## _REQUIRED)}; \
    }

#include "api.h"
#include "auth.h"
#include "channels.h"

#undef API_FIELDS_DEF
#undef API_FIELD
#undef API_REQUIRED_FIELD
#undef API_FIELDS_EXPAND
#undef API_FIELD_TO_ARG_DEF
#undef API_FIELD_TO_ARG_DEFS
#undef API_FIELD_TO_ARG
#undef API_FIELD_TO_ARGS
#undef API_FIELD_TO_INIT
#undef API_FIELD_TO_INITS
#undef API_OBJECT_RESULT_DECL
#undef API_OBJECT_DECL
#undef API_NAMESPACE_BEGIN
#undef API_NAMESPACE_END
#undef API_ENDPOINT
