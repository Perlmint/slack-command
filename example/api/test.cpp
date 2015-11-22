#include <iostream>
#include "slack/api.h"

int main(int argc, char* argv[])
{
  const std::string token;
  slack::api::Client client(token);

  auto result = client.api.test().foo("hello")();

  std::cout << result.get().foo();
  return 0;
}
