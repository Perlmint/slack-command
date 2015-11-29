#include <iostream>
#include "slack/api.h"

int main(int argc, char* argv[])
{
  const std::string token;
  slack::api::Client client(token);

  auto result = client.api.test().foo("hello")();

  auto foo = result.get().foo();
  if (foo)
  {
    std::cout << *foo;
  }
  else
  {
    std::cout << "<Not found>";
  }

  std::cout << std::endl;

  return 0;
}
