#include "Hlr_Requests_Controller.hpp"

Hlr_Requests_Controller::Hlr_Requests_Controller(boost::asio::io_service &io_service_):
    Hlr_Counter()
{
    Requests = new std::unordered_map<unsigned int, Request&>;
    condition_working = true;

    boost::thread worker(&Hlr_Requests_Controller::thread_worker, this);
}

void Hlr_Requests_Controller::add_Request(Request &req)
{
    std::lock_guard<std::mutex> guard(_mutex);
    std::pair<unsigned int, Request&> enumerated_req (hlr_get_num(), req);
    // for each request increase last request time - for timeout
    //last_request_income_utc_time = boost::posix_time::microsec_clock::universal_time();
    Requests->insert( enumerated_req );
}

void Hlr_Requests_Controller::thread_worker()
{
    // Goal to create separate thread for upcoming requests and return unholded requests back into main Requests list
    while(condition_working)
    {

       std::lock_guard<std::mutex> guard(_mutex);
       if (Requests->size() > 0)
       {

       }
    }

}
