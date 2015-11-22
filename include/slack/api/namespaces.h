#pragma once

#include "base.h"

#define API_PROPERTY(type, name) \
  private: \
    type _ ## name; \
  public: \
    const type &name() const \
    { \
      return _ ## name; \
    } \
    auto &name(const type &val) \
    { \
      _ ## name = val; \
      return *this; \
    }

    class ResultBase
    {
    public:
      API_PROPERTY(bool, ok);
    };

#define API_OBJECT_BEGIN(name) \
    class name : public ApiBase \
    { \
    public: \
      name(Client *client) : ApiBase(client) {}
#define API_OBJECT_RESULT_BEGIN() \
      class Result : ResultBase \
      { \
      public:

#define API_OBJECT_RESULT_END() }
#define API_OBJECT_END() }

#define API_NAMESPACE_BEGIN(name) \
    class name : public ApiBase \
    { \
    public: \
      name(Client *client) \
        : ApiBase(client) {}

#define API_NAMESPACE_END(); }

#define API_ENDPOINT(type, name) \
    type name() \
    { \
      return type{client}; \
    }

#include "api.h"
#include "auth.h"

#undef API_PROPERTY
#undef API_OBJECT_BEGIN
#undef API_OBJECT_RESULT_BEGIN
#undef API_OBJECT_RESULT_END
#undef API_OBJECT_END
#undef API_NAMESPACE_BEGIN
#undef API_NAMESPACE_END
#undef API_ENDPOINT
