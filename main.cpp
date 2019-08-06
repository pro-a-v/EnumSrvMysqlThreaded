// Is BASED ON
//
// async_udp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>

#include <signal.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <zdb/zdb.h>

#include "MsgProcesssor.hpp"
#include "DnsMessage.hpp"
#include "zbxsender.hpp"
#include "IOServer.h"


#include "RequestClass.hpp"




int main(int argc, char* argv[])
{

  try
  {


    boost::asio::io_service io_service;
    boost::asio::signal_set *signals_reload = new boost::asio::signal_set(io_service, SIGHUP);
    // server
    IOServer s(io_service, 5053);
    boost::asio::signal_set signals_stop(io_service, SIGINT, SIGTERM);
    signals_stop.async_wait(boost::bind(&boost::asio::io_service::stop, &io_service));
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cout << std::string("Exception:") << e.what() << std::endl;
  }




  return 0;
}
