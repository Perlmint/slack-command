#include <crow.h>
#include "slack/command.h"

int main()
{
  const std::string token = "";

  crow::SimpleApp app;
  slack::command::Processor commandProcessor;

  SLACK_COMMAND_REGIST(app, commandProcessor, "/");

  commandProcessor.route("/echo", "", "POST"_method,
                         [] SLACK_COMMAND_ARG(req) {
                           CROW_LOG_INFO << req.command;
                           return slack::command::Response().text(req.text);
                         });

  app.port(18080).run();
}
