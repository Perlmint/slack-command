#include <iostream>
#include "slack/api.h"

int main(int argc, char* argv[])
{
  const std::string token;
  slack::api::Client client(token);

  auto result = client.api.test().foo("hello")();

  result.wait();
  auto args = result.get().args();
  if (args)
  {
    std::cout << (*args).at("foo");
  }
  else
  {
    std::cout << "<Not found>";
  }

  std::cout << std::endl;

  return 0;
}
