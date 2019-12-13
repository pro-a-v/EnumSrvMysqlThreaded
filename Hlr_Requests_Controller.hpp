#ifndef HLR_REQUESTS_CONTROLLER_HPP
#define HLR_REQUESTS_CONTROLLER_HPP

#include <boost/asio.hpp>
#include <mutex>
#include <boost/thread.hpp>

#include "BlockingQueue.hpp"
#include "RequestClass.hpp"
#include "Hlr_Requests_HTTP11_Pipelined_Client.hpp"


class Hlr_Requests_Controller
{
public:
    Hlr_Requests_Controller(boost::asio::io_service &io_service_, boost::asio::ip::udp::socket *socket_udp_);
    void add_Request(Request *req);
    void process_Requests_worker();
    void process_Requests();
private:
    void thread_worker();
    boost::asio::io_service &io_service;

    BlockingQueue<Request *> income_queue;
    bool condition_working;
    std::mutex _mutex;
    Hlr_Requests_HTTP11_Pipelined_Client *HTTP11_Pipelined_Client;
    boost::asio::ip::udp::socket *socket_udp;
//
};

#endif // HLR_REQUESTS_CONTROLLER_HPP
