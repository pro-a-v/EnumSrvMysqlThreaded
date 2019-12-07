#include "Hlr_Requests_HTTP11_Pipelined_Client.hpp"


using boost::asio::ip::tcp;

Hlr_Requests_HTTP11_Pipelined_Client::Hlr_Requests_HTTP11_Pipelined_Client(boost::asio::io_service& io_service):
    resolver_(io_service), socket_(io_service)
  {
    // Form the request.
    std::ostream request_stream(&request_);
    request_stream << "POST /UssdService.svc HTTP/1.1\r\n";
    request_stream << "Host: 10.15.0.81:15335\r\n";
    request_stream << "Connection: Keep-Alive\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Type: application/json\r\n";
    request_stream << "Content-Length: 56\r\n\r\n";
    request_stream << "{\"srism_req\" : {\"msisdn\":\"380673769341\",\"request_id\":3}}\r\n";



    // Attempt a connection to endpoint
    boost::asio::ip::tcp::endpoint tcp_endpoint( boost::asio::ip::address::from_string("10.15.0.81"),  15335   );
    socket_.async_connect(tcp_endpoint,  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_connect, this,  boost::asio::placeholders::error));
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
      std::cerr << &response_;

      if (--number_of_requests >0 ){
        std::cerr << "\nread resp \n" ;
        boost::asio::async_read_until(socket_, response_, "\r\n\r\n",  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ));
        }

    }
    else if (err != boost::asio::error::eof)
    {
      std::cout << "Error: " << err << "\n";
    }
}
