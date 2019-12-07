#ifndef HLR_REQUESTS_CONTROLLER_HPP
#define HLR_REQUESTS_CONTROLLER_HPP

#include <boost/asio.hpp>
#include <mutex>
#include <boost/thread.hpp>

#include "RequestClass.hpp"
#include "Hlr_Requests_HTTP11_Pipelined_Client.hpp"


class Hlr_Requests_Controller: public Hlr_Counter
{
public:
    Hlr_Requests_Controller(boost::asio::io_service &io_service_);
    void add_Request(Request& req);
    void process_Requests_worker();
    void process_Requests();
private:
    void thread_worker();
    boost::asio::io_service io_service;

    std::unordered_map<unsigned int, Request&> *Requests;
    //boost::posix_time::ptime last_request_income_utc_time;

    bool condition_working;
    std::mutex _mutex;
//
};

#endif // HLR_REQUESTS_CONTROLLER_HPP
