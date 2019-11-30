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

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <memory>
#include "Customsink.hpp"



int main(int argc, char* argv[])
{

       std::unique_ptr<g3::LogWorker> logworker{ g3::LogWorker::createLogWorker() };
       //auto logworker = g3::LogWorker::createLogWorker();
       auto handle = logworker->addDefaultLogger("enum.log", "/var/log/enum");
       auto sinkHandle = logworker->addSink(std::make_unique<CustomSink>(), &CustomSink::ReceiveLogMessage);
       initializeLogging(logworker.get());


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
    LOG(WARNING) << std::string("Exception:") << e.what() << std::endl;
  }




  return 0;
}
