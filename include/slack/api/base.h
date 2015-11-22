#pragma once

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
      void run(const char *url)
      {
      }

      Client *client = nullptr;
    };
  }
}

#undef SLACK_API_BASE_URL
