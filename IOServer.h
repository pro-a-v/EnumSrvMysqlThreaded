#ifndef IOSERVER_H
#define IOSERVER_H


#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <boost/atomic.hpp>
#include <memory>

#include "udpmessage.hpp"
#include "RequestClass.hpp"
#include "MsgProcesssor.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "BlockingQueue.hpp"
#include "DbData.hpp"

#include "DbDataDefaultCache.hpp"
#include "DbDataDaughterCache.hpp"
#include "DbData_ProcessingType_Cache.hpp"
#include "DbDataClientsDenyList.hpp"


#define UDP_MSG_SIZE 512

#include <g3log/g3log.hpp>

using boost::asio::ip::udp;


#include <boost/thread.hpp>
#include <deque>
#include <fstream>

#include <iostream>
//static boost::mutex s_iomutex;



class IOServer
{
public:
  IOServer(boost::asio::io_service& io_service, short port);
  ~IOServer();
  void do_receive();


private:
  udp::socket socket_;
  udp::endpoint sender_endpoint_;
  enum { max_length = UDP_MSG_SIZE };
  char data_[UDP_MSG_SIZE];


  std::string currentDateTime();

  BlockingQueue<Request *> income_queue;


  boost::atomic<bool> done;

  void RequestConsumerWorker();
  void SendErrorAnswer(DnsMessage *NS ,Request *req);
  void SendAccessDenyAnswer(DnsMessage *NS ,Request *req);
  bool ProcessDBRequest(DbData &dbd, DnsMessage *NS ,Request *req);

  const char *testURL = "mysql://localhost:3306/enum?user=enumrw&password=Enumrw147!";
  URL_T url;
  ConnectionPool_T pool;
  DbDataDefaultCache *DefaultDataCache;
  DbDataDaughterCache *DaughterDataCache;
  DbDataClientsDenyList *ClientsDenyList;
  DbData_ProcessingType_Cache *ProcessingType_Cache;

};

#endif // IOSERVER_H
