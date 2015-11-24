#pragma once

#include <functional>
#include <boost/optional.hpp>

#define SLACK_API_BASE_URL "https://slack.com"

namespace slack
{
  namespace api
  {
    class Client;

    class ApiBase
    {
    public:
      ApiBase(Client *client)
        : client(client)
      {
      }

    protected:
      void run(const std::string &url, const std::function<void(const std::string&)> & callback)
      {
      }

      Client *client = nullptr;
    };
  }
}

#undef SLACK_API_BASE_URL
