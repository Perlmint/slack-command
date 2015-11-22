#pragma once

namespace slack
{
  namespace api
  {
    API_OBJECT_BEGIN(ApiTest)
      API_OBJECT_RESULT_BEGIN()
        API_PROPERTY(std::string, foo);
      API_OBJECT_RESULT_END();

      std::future<Result> operator()()
      {
        std::promise<Result> promise;
        run("/api/api.test");
        return promise.get_future();
      }

      void operator()(const std::function<void(Result &)> &callback)
      {
      }

      API_PROPERTY(std::string, foo);
      API_PROPERTY(std::string, error);
    private:
      void parse(std::promise<Result> promise)
      {
      }

      void parse(Result &ret)
      {
      }
    API_OBJECT_END();

    API_NAMESPACE_BEGIN(Api)
      API_ENDPOINT(ApiTest, test);
    API_NAMESPACE_END();
  }
}
