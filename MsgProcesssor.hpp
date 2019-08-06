#ifndef MSGPROCESSSOR_H
#define MSGPROCESSSOR_H

#include <boost/asio.hpp>
#include <exception>
#include <assert.h>
#include <boost/lockfree/queue.hpp>
#include "RequestClass.hpp"

#include "DnsMessage.hpp"

using boost::asio::ip::udp;

class MsgProcesssor
{
public:
    MsgProcesssor(boost::lockfree::queue<Request *, boost::lockfree::capacity<1000>> *income_queue, boost::atomic<bool> *done);
    void Run();
    std::string currentDateTime();
private:
      boost::lockfree::queue<Request *, boost::lockfree::capacity<1000>> *income_queue_;
      boost::atomic<bool> *done_;
};

#endif // MSGPROCESSSOR_H
