#include "Hlr_Requests_HTTP11_Pipelined_Client.hpp"


using boost::asio::ip::tcp;

Hlr_Requests_HTTP11_Pipelined_Client::Hlr_Requests_HTTP11_Pipelined_Client(boost::asio::io_service& io_service, size_t requests_count,  BlockingQueue<Request *> *income_queue):
    resolver_(io_service), socket_(io_service), income_queue_(income_queue)
  {
    for (size_t i=0; i < requests_count; i++)
    {
          requests.push_back(income_queue->pop());
    }

    client_start_utc_time = boost::posix_time::microsec_clock::universal_time();

    create_http_requests();

    // Attempt a connection to endpoint
    boost::asio::ip::tcp::endpoint tcp_endpoint( boost::asio::ip::address::from_string("10.15.0.81"),  15335   );
    socket_.async_connect(tcp_endpoint,  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_connect, this,  boost::asio::placeholders::error));
}

void Hlr_Requests_HTTP11_Pipelined_Client::create_http_requests()
{

    std::ostream request_stream(&request_);
    for (auto const &Req: requests)
    {
    // Form the request.


    request_stream << "POST /UssdService.svc HTTP/1.1\r\n";
    request_stream << "Host: 10.15.0.81:15335\r\n";
    request_stream << "Connection: Keep-Alive\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Type: application/json\r\n";

    std::string rest = std::string("{\"srism_req\" : {\"msisdn\":\"") + Req->phone_number + std::string("\",\"request_id\":") + std::to_string(Req->uid) + std::string("}}\r\n");

    request_stream << "Content-Length: "<< rest.size() <<"\r\n\r\n";
    request_stream << rest;
    }
}

void Hlr_Requests_HTTP11_Pipelined_Client::handle_connect(const boost::system::error_code& err)
  {
    if (!err)
    {
      // The connection was successful. Send the request.
      boost::asio::async_write(socket_, request_,  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_write_request, this, boost::asio::placeholders::error));
    }
    else
    {
      std::cout << "Error: " << err.message() << "\n";
    }
  }

void Hlr_Requests_HTTP11_Pipelined_Client::handle_write_request(const boost::system::error_code& err)
  {
    if (!err)
    {
      // Read the response
      boost::asio::async_read_until(socket_, response_, "\r\n\r\n",  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ));
    }
    else
    {
      std::cout << "Error: " << err.message() << "\n";
    }
  }





void Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce(const boost::system::error_code& err, std::size_t bytes_transferred)
{
    if (!err)
    {
      // Write all of the data that has been read so far.


      char buf[4096], *method, *path;
      int pret, minor_version, status;
      struct phr_header headers[100];
      size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers, msg_len, last_len = 0;
      ssize_t rret;
      const char *msg;
      std::string target{buffers_begin(response_.data()), buffers_end(response_.data())};
      pret = phr_parse_response(target.c_str(), target.size(), &minor_version, &status, &msg, &msg_len, headers, &num_headers, last_len);

      std::cerr << msg;
      //  boost::asio::async_read_until(socket_, response_, "\r\n\r\n",  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ));

    }
    else if (err != boost::asio::error::eof)
    {
      std::cout << "Error: " << err << "\n";
    }
}
