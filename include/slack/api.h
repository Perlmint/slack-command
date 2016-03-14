#pragma once

#include <future>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <memory>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/asio/io_service.hpp>
#include <boost/asio/use_future.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#pragma clang diagnostic pop

#include "api/namespaces.h"

namespace slack
{
  namespace api
  {
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    using message_ptr = websocketpp::config::asio_tls_client::message_type::ptr;
    using context_ptr = websocketpp::lib::shared_ptr<boost::asio::ssl::context>;
    using websocketpp::lib::bind;

    class Client : public std::enable_shared_from_this<Client>
    {
    public:
      using this_type = Client;
      using client_t = websocketpp::client<websocketpp::config::asio_tls_client>;
      Client(const std::string &token)
        : api(this)
        , auth(this)
        , channels(this)
        , rtm(this)
        , counter(0)
        , token(token)
        , work(service)
      {
        client.init_asio();

        // Register our handlers
        client.set_message_handler(bind(&this_type::on_message, this, _1, _2));
        client.set_open_handler(bind(&this_type::on_open, this, _1));
        client.set_tls_init_handler(bind(&this_type::on_tls_init, this, _1));
        client.set_close_handler(bind(&this_type::on_close, this, _1));
        client.set_fail_handler(bind(&this_type::on_fail, this, _1));

        io_thread = std::thread(std::bind(static_cast<size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run), &service));
      }
      virtual ~Client()
      {
      }

      Api api;
      Auth auth;
      Channels channels;
      Rtm rtm;

      boost::asio::io_service &Service()
      {
        return service;
      }

      std::future<void> rtm_begin()
      {
        std::string url = rtm.start(token)().get().url().value();
        websocketpp::lib::error_code ec;
        client_t::connection_ptr con = client.get_connection(url, ec);

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
        endPromise.set_value();
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

      context_ptr on_tls_init(websocketpp::connection_hdl) {
        context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);

        try
        {
          ctx->set_options(boost::asio::ssl::context::default_workarounds |
                           boost::asio::ssl::context::no_sslv2 |
                           boost::asio::ssl::context::no_sslv3 |
                           boost::asio::ssl::context::single_dh_use);
        }
        catch (std::exception& e)
        {
          std::cout << e.what() << std::endl;
        }
        return ctx;
      }

      const std::string &getToken()
      {
        return token;
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

      boost::asio::io_service service;
      boost::asio::io_service::work work;
      std::thread io_thread;
      client_t client;
    };
  }
}

#include "api/base_impl.h"
