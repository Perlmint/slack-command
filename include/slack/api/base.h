#pragma once

#include <set>
#include <functional>
#include <boost/optional.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#pragma clang diagnostic pop
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#define SLACK_API_SERVER "slack.com"

namespace slack
{
  namespace api
  {
    class Client;
    using jsonDoc = rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator>;

    class ApiBase
    {
    public:
      ApiBase(Client *client)
        : client(client)
      {
      }

    protected:
      Client *client = nullptr;
      class HttpClient : public std::enable_shared_from_this<HttpClient>
      {
      private:
        using tcp = boost::asio::ip::tcp;
        using this_type = HttpClient;
      public:
        HttpClient(ApiBase *base,
                   boost::asio::io_service &service,
                   const std::string& path,
                   const jsonDoc &params,
                   const std::function<void(const std::string &)> &callback)
          : resolver(service)
          , ssl_context(boost::asio::ssl::context::sslv23)
          , socket(service, ssl_context)
          , response_callback(callback)
        {
          //socket.set_verify_mode(boost::asio::ssl::verify_peer);
          //socket.set_verify_callback(std::bind(&this_type::verify_certificate, this, std::placeholders::_1, std::placeholders::_2));

          std::ostringstream params_stream;
          bool first = true;
          for (auto itr = params.MemberBegin(), end = params.MemberEnd(); itr != end; ++itr)
          {
            if (!first)
            {
              params_stream << "&";
            }
            params_stream << itr->name.GetString() << "=";
            const auto &val = itr->value;
            if (val.IsInt())
            {
              params_stream << val.GetInt();
            }
            else if (val.IsUint())
            {
              params_stream << val.GetUint();
            }
            else if (val.IsInt64())
            {
              params_stream << val.GetInt64();
            }
            else if (val.IsUint64())
            {
              params_stream << val.GetUint64();
            }
            else if (val.IsDouble())
            {
              params_stream << val.GetDouble();
            }
            else if (val.IsString())
            {
              const auto str = val.GetString();
              for (int i = 0, length = val.GetStringLength(); i < length; i++)
              {
                char c = str[i];
                if (c >= 0 && (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')) {
                  params_stream.put(c);
                  continue;
                }
                int character = 0;
                int utf8len = 0;
                for (utf8len = 0;utf8len < 5; utf8len++)
                {
                  if ((c & (0x80 >> i)) == 0)
                  {
                    break;
                  }
                }
                if (utf8len == 0)
                {
                  character = c;
                }
                else
                {
                  character = (0x7F >> utf8len);
                  for (; utf8len > 1; utf8len--)
                  {
                    i++;
                    character = (character << 6) | (str[i] & 0x3F);
                  }
                }
                params_stream << std::uppercase << "%" << std::setw(2) << character << std::nouppercase;
              }
            }
            first = false;
          }

          auto params_str = params_stream.str();

          std::ostream request_stream(&request);
          request_stream << "POST /" << path << "?token=" << base->getToken() << " HTTP/1.1\r\n"
                         << "Host: " << SLACK_API_SERVER << "\r\n"
                         << "Accept: application/json\r\n"
                         << "User-Agent: slacpp\r\n"
                         << "Content-Type: application/x-www-form-urlencoded\r\n"
                         << "Content-Length: " << params_str.length() << "\r\n"
                         << "Connection: Close\r\n"
                         << "\r\n"
                         << params_str;
        }

        void run()
        {
          tcp::resolver::query query(SLACK_API_SERVER, "https");
          resolver.async_resolve(query,
              boost::bind(&this_type::handle_resolve, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::iterator));
        }

      private:
        void throwException(const char *format, const boost::system::error_code &err)
        {
          dispose();
          throw new std::logic_error((boost::format(format) % err.message()).str());
        }

        bool verify_certificate(bool preverified,
                                boost::asio::ssl::verify_context& ctx)
        {
          return preverified;
        }

        void handle_resolve(const boost::system::error_code& err,
                            tcp::resolver::iterator endpoint_iterator)
        {
          if (!err)
          {
            // Attempt a connection to each endpoint in the list until we
            // successfully establish a connection.
            boost::asio::async_connect(socket.lowest_layer(), endpoint_iterator,
                boost::bind(&this_type::handle_connect, shared_from_this(),
                  boost::asio::placeholders::error));
          }
          else
          {
            throwException("Resolve Failed : %1%", err);
          }
        }

        void handle_connect(const boost::system::error_code& err)
        {
          if (!err)
          {
            // The connection was successful. Send the request.
            socket.async_handshake(boost::asio::ssl::stream_base::client,
              boost::bind(&this_type::handle_handshake, this,
                boost::asio::placeholders::error));
          }
          else
          {
            throwException("Error: %1%", err);
          }
        }

        void handle_handshake(const boost::system::error_code& error)
        {
          if (!error)
          {
            boost::asio::async_write(socket, request,
              boost::bind(&this_type::handle_write_request, shared_from_this(),
                boost::asio::placeholders::error));
          }
          else
          {
            throwException("Handshake failed: %1%", error);
          }
        }

        void handle_write_request(const boost::system::error_code& err)
        {
          if (!err)
          {
            // Read the response status line. The response_ streambuf will
            // automatically grow to accommodate the entire line. The growth may be
            // limited by passing a maximum size to the streambuf constructor.
            boost::asio::async_read_until(socket, response, "\r\n",
                boost::bind(&this_type::handle_read_status_line, shared_from_this(),
                  boost::asio::placeholders::error));
          }
          else
          {
            throwException("Write Request Failed : %1%", err);
          }
        }

        void handle_read_status_line(const boost::system::error_code& err)
        {
          if (!err)
          {
            // Check that response is OK.
            std::istream response_stream(&response);
            std::string http_version;
            response_stream >> http_version;
            unsigned int status_code;
            response_stream >> status_code;
            std::string status_message;
            std::getline(response_stream, status_message);
            if (!response_stream || http_version.substr(0, 5) != "HTTP/")
            {
              throw new std::logic_error("Invalid response");
              return;
            }
            if (status_code != 200)
            {
              throw new std::logic_error((boost::format("Response returned with status code %d") % status_code).str());
              return;
            }

            // Read the response headers, which are terminated by a blank line.
            boost::asio::async_read_until(socket, response, "\r\n\r\n",
                boost::bind(&this_type::handle_read_headers, shared_from_this(),
                  boost::asio::placeholders::error));
          }
          else
          {
            throwException("Read Status Failed : ", err);
          }
        }

        void handle_read_headers(const boost::system::error_code& err)
        {
          if (!err)
          {
            // Process the response headers.
            std::istream response_stream(&response);
            std::string header;
            while (std::getline(response_stream, header) && header != "\r")
            {
              auto index = header.find(':');
              response_header.insert(
                std::make_pair(
                  header.substr(0, index),
                  header.substr(index + 2, header.length() - (index + 3))));
            }

            // Start reading remaining data until EOF.
            boost::asio::async_read(socket, response,
                boost::asio::transfer_at_least(1),
                boost::bind(&this_type::handle_read_content, shared_from_this(),
                  boost::asio::placeholders::error));
          }
          else
          {
            throwException("Read header error : %1%", err);
          }
        }

        void handle_read_content(const boost::system::error_code& err)
        {
          bool read_next = false;
          if (!err)
          {
            read_next = true;
          }
          else if (err != boost::asio::error::eof)
          {
            throwException("Read Content Error: ", err);
          }
          else
          {
            if (response.size() > 0)
            {
              // Write all of the data that has been read so far.
              std::istreambuf_iterator<char> begin(&response);
              while(true)
              {
                if (*begin == '{')
                {
                  break;
                }
                ++begin;
              }
              std::string body{
                begin,
                std::istreambuf_iterator<char>()};
              body.erase(body.rfind('}') + 1);
              response_callback(body);
            }
          }

          if (read_next)
          {
            // Continue reading remaining data until EOF.
            boost::asio::async_read(socket, response,
                boost::asio::transfer_at_least(1),
                boost::bind(&this_type::handle_read_content, shared_from_this(),
                  boost::asio::placeholders::error));
          }
        }

        void dispose()
        {
          base->clients.erase(shared_from_this());
        }

        ApiBase *base;
        tcp::resolver resolver;
        boost::asio::ssl::context ssl_context;
        boost::asio::ssl::stream<tcp::socket> socket;
        boost::asio::streambuf request;
        boost::asio::streambuf response;
        std::map<std::string, std::string> response_header;
        std::function<void(const std::string &)> response_callback;
        long content_size = -1;
      };

      void run(const std::string &path,
               const jsonDoc &params,
               const std::function<void(const std::string&)> & callback);
      
      const std::string &getToken();

      std::set<std::shared_ptr<HttpClient>> clients;
    };
  }
}

#undef SLACK_API_BASE_URL
