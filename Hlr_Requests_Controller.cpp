#include "Hlr_Requests_Controller.hpp"

Hlr_Requests_Controller::Hlr_Requests_Controller(boost::asio::io_service &io_service_, boost::asio::ip::udp::socket *socket_udp_): io_service(io_service_), socket_udp(socket_udp_)
{

    condition_working = true;

    boost::thread worker(&Hlr_Requests_Controller::thread_worker, this);
}

void Hlr_Requests_Controller::add_Request(Request *req)
{
    income_queue.push(req);
}

void Hlr_Requests_Controller::thread_worker()
{
    // Goal to create separate thread for upcoming requests and return unholded requests back into main Requests list
    while(condition_working)
    {

       std::lock_guard<std::mutex> guard(_mutex);
       if (income_queue.size() > 0)
       {
            // std::cout << income_queue.size() << std::endl;
            HTTP11_Pipelined_Client = new Hlr_Requests_HTTP11_Pipelined_Client(io_service, income_queue.size(), &income_queue, socket_udp);

       }
       boost::this_thread::sleep_for(boost::chrono::milliseconds(30));
    }

}
