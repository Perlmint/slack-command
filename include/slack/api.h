#pragma once

#include <future>
#include <unordered_map>
#include <atomic>
#include <memory>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_future.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include "api/namespaces.h"

namespace slack
{
  namespace api
  {
    static const std::string rtm_start_endpoint{"https://slack.com/api/rtm.start"};
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    using message_ptr = websocketpp::config::asio_tls_client::message_type::ptr;
    using websocketpp::lib::bind;

    class Client : public std::enable_shared_from_this<Client>
    {
    public:
      using this_type = Client;
      using client_t = websocketpp::client<websocketpp::config::asio_tls_client>;
      Client(const std::string &token)
        : token(token)
        , counter(0)
        , api(this)
        , auth(this)
      {
        client.init_asio();

        // Register our handlers
        client.set_message_handler(bind(&this_type::on_message, this, _1, _2));
        client.set_open_handler(bind(&this_type::on_open, this, _1));
        client.set_close_handler(bind(&this_type::on_close, this, _1));
        client.set_fail_handler(bind(&this_type::on_fail, this, _1));
      }

      Api api;
      Auth auth;

      std::future<void> rtm_begin()
      {
        websocketpp::lib::error_code ec;
        client_t::connection_ptr con = client.get_connection(rtm_start_endpoint + "?token=" + token, ec);

        if (ec)
        {
          client.get_alog().write(websocketpp::log::alevel::app,ec.message());
        }

        client.connect(con);

        client.run();

        return endPromise.get_future();
      }

      std::future<bool> send_simple_message(const std::string &text)
      {
        auto &promise = messagePromises.insert(std::make_pair(get_count(), std::promise<bool>())).first->second;

        std::async(std::launch::async, [this, &promise]() {
            
          });

        return promise.get_future();
      }

      void on_fail(websocketpp::connection_hdl hdl)
      {
      }

      void on_open(websocketpp::connection_hdl hdl)
      {
      }

      void on_message(websocketpp::connection_hdl hdl, message_ptr msg)
      {
        std::cout << msg->get_payload() << std::endl;
      }

      void on_close(websocketpp::connection_hdl)
      {
        endPromise.set_value();
      }

    private:
      int get_count()
      {
        return counter;
      }

      std::promise<void> endPromise;

      std::atomic<int> counter;
      std::unordered_map<int, std::promise<bool>> messagePromises;

      std::string token;

      client_t client;
    };
  }
}
