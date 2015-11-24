#include <crow.h>
#include <map>

#define SLACK_COMMAND_REGIST(crow_app, processor, url) processor.regist(CROW_ROUTE(crow_app, url))
#define SLACK_COMMAND_ARG(request_name) (const slack::command::Request& request_name) -> slack::command::Response

namespace slack
{
  namespace command
  {
    enum class ResponseType
    {
      in_channel,
      ephemeral
    };

    std::string toString(const ResponseType &respType)
    {
      switch(respType)
      {
      case ResponseType::in_channel:
        return "in_channel";
      default:
        return "ephemeral";
      }
    }

    class Request
    {
    public:
      Request()
      {
      }

      void read(const crow::query_string &getParams)
      {
#define GET_PARAM(name) { \
          const char *str = getParams.get(#name); \
          if (str != nullptr) \
          { \
            name = str; \
          } \
        }
        GET_PARAM(token);
        GET_PARAM(team_id);
        GET_PARAM(team_domain);
        GET_PARAM(channel_id);
        GET_PARAM(channel_name);
        GET_PARAM(user_id);
        GET_PARAM(user_name);
        GET_PARAM(command);
        GET_PARAM(text);
        GET_PARAM(response_url);
#undef GET_PARAM
      }

      std::string token;
      std::string team_id;
      std::string team_domain;
      std::string channel_id;
      std::string channel_name;
      std::string user_id;
      std::string user_name;
      std::string command;
      std::string text;
      std::string response_url;
    };

    class Response
    {
    public:
      Response()
      {
      }

      Response(const std::string &txt)
        : _text(txt)
      {
      }

      Response &text(const std::string &txt)
      {
        _text = txt;
      }

      std::string toString()
      {
        return "{\"response_type\":\"" + command::toString(responseType) + "\",\"text\":\"" + _text + "\"}";
      }

    private:
      std::string _text;
      ResponseType responseType = ResponseType::ephemeral;
    };

    class Processor
    {
    public:
      Processor() {}

      template<typename RULE>
      RULE &regist(RULE &rule)
      {
        rule.methods("GET"_method, "POST"_method)(getHandler());
        return rule;
      }

      using handler_t = std::function<Response(const Request&)>;
      using common_handler_t = std::function<void(const crow::request&, crow::response &)>;

      common_handler_t getHandler()
      {
        return std::bind(&Processor::commonHandler,
                         this,
                         std::placeholders::_1,
                         std::placeholders::_2);
      }

      void commonHandler(const crow::request &req,
                         crow::response &res)
      {
        Request request;
        if (req.method == "GET"_method)
        {
          request.read(req.url_params);
        }
        else if (req.method == "POST"_method)
        {
          auto typeItr = req.headers.find("Content-Type");
          if (typeItr == req.headers.end())
          {
          }
          if (typeItr->second.compare("application/x-www-form-urlencoded") == 0)
          {
            request.read(crow::query_string("?" + req.body));
          }
          // TODO: parse other types...
        }
        else
        {
          CROW_LOG_ERROR << "Invalid method";
          res.code = 405;
          res.end();
          return;
        }

        auto itr = handlerMap.find(std::make_pair(request.command, request.token));
        if (itr == handlerMap.end())
        {
          itr = handlerMap.find(std::make_pair(request.command, ""));
        }
        if (itr == handlerMap.end())
        {
          CROW_LOG_ERROR << "Not available command or token";
          res.code = 404;
          res.end();
          return;
        }

        if (itr->second.first != req.method)
        {
          res.code = 405;
          res.end();
          return;
        }

        auto resp = itr->second.second(request);
        res.set_header("content-type", "application/json");
        res.write(resp.toString());
        res.end();
      }

      void route(const std::string &command,
                      const std::string &token,
                      crow::HTTPMethod method,
                      const handler_t &f)
      {
        handlerMap.insert(std::make_pair(std::make_pair(command, token),
                                         std::make_pair(method, f)));
      }

    private:
      std::map<std::pair<std::string, std::string>,
               std::pair<crow::HTTPMethod, handler_t> > handlerMap;
    };
  }
}
