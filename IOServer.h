#ifndef IOSERVER_H
#define IOSERVER_H


#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
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

#include "DbData.hpp"

#include "DbDataDefaultCache.hpp"
#include "DbDataDaughterCache.hpp"
#include "DbDataClientsDenyList.hpp"

#define UDP_MSG_SIZE 512

#include <g3log/g3log.hpp>

using boost::asio::ip::udp;


#include <boost/thread.hpp>
#include <deque>
#include <fstream>

#include <iostream>
//static boost::mutex s_iomutex;

template <typename T> class BlockingQueue {
  public:
    explicit BlockingQueue() : _buffer() {
    }

    void push(const T &elem) {
        boost::unique_lock<boost::mutex> lock(_mutex);
        _pop_event.wait(lock, [&] { return _buffer.size() < _capacity; });
        _buffer.push_back(elem);
        _push_event.notify_one(); // notifies one of the waiting threads which are blocked on the queue
        // assert(!_buffer.empty());
    }

    T pop() {
        boost::unique_lock<boost::mutex> lock(_mutex);
        _push_event.wait(lock, [&] { return _buffer.size() > 0; });

        T elem = _buffer.front();
        _buffer.pop_front();
        _pop_event.notify_one();
        return elem;
    }

    T size() {
        boost::unique_lock<boost::mutex> lock(_mutex);
        return _buffer.size();
    }

  private:
    boost::mutex _mutex;
    boost::condition_variable _push_event, _pop_event;
    std::deque<T> _buffer;
    size_t _capacity = 4096;
};


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
  //boost::lockfree::queue<Request *, boost::lockfree::fixed_sized<true> , boost::lockfree::capacity<4096>> income_queue;
  //boost::lockfree::queue<Request *, boost::lockfree::capacity<1000>> income_queue;

  BlockingQueue<Request *> income_queue;
  BlockingQueue<Request *> hlr_queue;

  boost::atomic<bool> done;

  void RequestConsumerWorker();
  void SendErrorAnswer(DnsMessage *NS ,Request *req);
  void SendAccessDenyAnswer(DnsMessage *NS ,Request *req);
  bool ProcessDBRequest(DbData &dbd, DnsMessage *NS ,Request *req);

  const char *testURL = "mysql://localhost:3306/enum?user=enumrw&password=Qazxsw12345679]";
  URL_T url;
  ConnectionPool_T pool;
  DbDataDefaultCache *DefaultDataCache;
  DbDataDaughterCache *DaughterDataCache;
  DbDataClientsDenyList *ClientsDenyList;

};

#endif // IOSERVER_H
