#ifndef HLR_REQUESTS_HTTP11_PIPELINED_CLIENT_HPP
#define HLR_REQUESTS_HTTP11_PIPELINED_CLIENT_HPP
#include <iostream>
#include <unordered_map>
#include "Hlr_Counter.hpp"
#include "RequestClass.hpp"
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/bind.hpp>


class Hlr_Requests_HTTP11_Pipelined_Client
{
public:
    Hlr_Requests_HTTP11_Pipelined_Client(boost::asio::io_service &io_service);
private:
  void perform_http_requests();
  void handle_connect(const boost::system::error_code& err);
  void handle_write_request(const boost::system::error_code& err);
  void handle_read_responce(const boost::system::error_code& err, std::size_t bytes_transferred);

  boost::asio::ip::tcp::resolver resolver_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::streambuf request_;
  boost::asio::streambuf response_;

  int number_of_requests = 0;

};

#endif // HLR_REQUESTS_HTTP11_PIPELINED_CLIENT_HPP
