#include "slack/api.h"

int main(int argc, char* argv[])
{
  slack::api::Client client("");

  client.rtm_begin().wait();

  return 0;
}
