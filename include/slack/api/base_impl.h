#pragma once

void slack::api::ApiBase::run(const std::string &path,
                              const jsonDoc &params,
                              const std::function<void(const std::string&)> & callback)
{
  auto httpClient = std::shared_ptr<HttpClient>(new HttpClient(this, client->Service(), path, params, callback));
  clients.insert(httpClient);
  httpClient->run();
}

const std::string &slack::api::ApiBase::getToken()
{
  return client->getToken();
}
