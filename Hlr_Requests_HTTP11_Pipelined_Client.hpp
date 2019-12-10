#ifndef HLR_REQUESTS_HTTP11_PIPELINED_CLIENT_HPP
#define HLR_REQUESTS_HTTP11_PIPELINED_CLIENT_HPP
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <bits/stdc++.h>

#include "DnsMessage.hpp"
#include "BlockingQueue.hpp"
#include "RequestClass.hpp"
#include "picohttpparser.hpp"


class Hlr_Requests_HTTP11_Pipelined_Client
{
public:
    Hlr_Requests_HTTP11_Pipelined_Client(boost::asio::io_service &io_service, size_t requests_count,  BlockingQueue<Request *> *income_queue);
private:
  void create_http_requests();
  void handle_connect(const boost::system::error_code& err);
  void handle_write_request(const boost::system::error_code& err);
  void handle_read_responce(const boost::system::error_code& err, std::size_t bytes_transferred);

  boost::asio::ip::tcp::resolver resolver_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::streambuf request_;
  boost::asio::streambuf response_;


  boost::posix_time::ptime client_start_utc_time;

  BlockingQueue<Request *> *income_queue_;
  std::vector<Request *> requests;

};

#endif // HLR_REQUESTS_HTTP11_PIPELINED_CLIENT_HPP
