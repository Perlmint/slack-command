#include <crow.h>
#include <map>

#define SLACK_COMMAND_REGISTER(app, url, processor) processor.registerRule(CROW_ROUTE(app, url))
#define SLACK_COMMAND_ROUTE(processor, command) processor.addHandler(command)
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

      void read(const std::string &postBody)
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
      Processor(const std::string &token)
        : token(token)
      {
      }

      Processor(const std::string &token, crow::HTTPMethod method)
        : Processor(token)
      {
        this->method = method;
        methodValidity = method == "GET"_method || method == "POST"_method;
      }

      using handler_t = std::function<Response(const Request&)>;
      using common_handler_t = std::function<void(const crow::request&, crow::response &)>;

      template<typename RULE>
      RULE &registerRule(RULE &rule)
      {
        if (methodValidity)
        {
          rule.methods(method);
        }

        rule(getHandler());
        return rule;
      }

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
        // Error! HTTPMethod not matching
        if (methodValidity && req.method != method)
        {
          CROW_LOG_ERROR << "Wrong method request";
          return;
        }

        CROW_LOG_INFO << "request";
        Request request;
        if (req.method == "GET"_method)
        {
          request.read(req.url_params);
        }
        else
        {
          request.read(req.body);
        }

        if (request.token != token)
        {
        }

        auto itr = handlerMap.find(request.command);
        if (itr != handlerMap.end())
        {
          auto resp = itr->second(request);
          res.set_header("content-type", "application/json");
          res.write(resp.toString());
          res.end();
        }
      }

      // Temporal object
      class HandlerAdder
      {
      public:
      HandlerAdder(Processor &processor,
                   const std::string &command)
          : processor(processor)
          , command(command)
        {
        }

        void operator()(const handler_t &f)
        {
          processor.doAddHandler(command, f);
        }

      private:
        std::string command;
        Processor &processor;
      };

      HandlerAdder addHandler(const std::string &command)
      {
        return HandlerAdder(*this, command);
      }

      void doAddHandler(const std::string &command, const handler_t &f)
      {
        handlerMap.insert(std::make_pair(command, f));
      }

    private:
      const std::string token;
      std::map<std::string, handler_t> handlerMap;
      // initialize with invalid method
      crow::HTTPMethod method = static_cast<crow::HTTPMethod>(-1);
      bool methodValidity = false;
    };
  }
}
