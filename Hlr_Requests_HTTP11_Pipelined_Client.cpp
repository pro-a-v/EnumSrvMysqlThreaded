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
    //boost::asio::ip::tcp::endpoint tcp_endpoint( boost::asio::ip::address::from_string("10.15.0.81"),  15335   );
    boost::asio::ip::tcp::endpoint tcp_endpoint( boost::asio::ip::address::from_string("127.0.0.1"),  15335   );
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
      boost::asio::async_read_until(socket_, response_, "\r\n\r\n",  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce_headers, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ));
    }
    else
    {
      std::cout << "Error: " << err.message() << "\n";
    }
  }





void Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce_headers(const boost::system::error_code& err, std::size_t bytes_transferred)
{
    if (!err)
    {
      // Write all of the data that has been read so far.
        // Check that response is OK.
      std::istream response_stream(&response_);
      std::string http_version;
      response_stream >> http_version;
      //std::cout << "V : " << http_version << std::endl;

      unsigned int status_code;
      response_stream >> status_code;

      if (status_code == 200)
      {
          std::string status_message;
          std::getline(response_stream, status_message);


          std::string header;
          std::string content_length = std::string("content-length:");
          std::string content_length_value = "0";
          std::size_t content_length_value_int = 0;

          //read the headers.
          while (std::getline(response_stream, header) && header != "\r")
          {
                boost::algorithm::to_lower(header);
                std::size_t found = header.find(content_length);
                if ( found != std::string::npos)
                {
                      content_length_value =  header.substr(found + content_length.size() ) ;
                      boost::trim(content_length_value);
                      content_length_value_int = boost::lexical_cast<std::size_t>(content_length_value);
                }
          }

          if ( content_length_value_int > 0 )
          {
              // read body
              std::cout << " Body size: " << content_length_value_int;
              std::cout << " Data size: " << response_.size();
              std::cout << " Data capacity: " << response_.capacity();
              boost::asio::async_read(socket_, response_, boost::asio::transfer_at_least(content_length_value_int),  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce_body, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ));
          }
      }

    }
    else if (err != boost::asio::error::eof)
    {
      std::cout << "Error: " << err << "\n";
    }
}

void Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce_body(const boost::system::error_code &err, std::size_t bytes_transferred)
{
    if (!err)
    {
       std::ostringstream ss;
       ss << &response_;
       std::string data(ss.str());
       std::cout << data << std::endl;
    }
    else if (err != boost::asio::error::eof)
    {
      std::cout << "Error: " << err << "\n";
    }
    
}
