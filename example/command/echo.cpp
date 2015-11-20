#include <crow.h>
#include "slack/command.h"

int main()
{
  const std::string token = "";

  crow::SimpleApp app;
  slack::command::Processor commandProcessor(token, "GET"_method);

  SLACK_COMMAND_REGISTER(app, "/", commandProcessor);
  SLACK_COMMAND_ROUTE(commandProcessor, "/echo")
    ([] SLACK_COMMAND_ARG(req) {
      CROW_LOG_INFO << req.command;
      return slack::command::Response().text(req.text);
    });

    app.port(18080).run();
}
