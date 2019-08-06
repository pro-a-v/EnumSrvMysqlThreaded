#ifndef REQUESTDATACLASS_HPP
#define REQUESTDATACLASS_HPP

#include <boost/asio.hpp>
#include <string.h>

class Request
{
public:
    Request(char *data, int len, boost::asio::ip::udp::endpoint sender_endpoint);

    std::string raw_data;
    boost::asio::ip::udp::endpoint sender_endpoint_;
};


#endif // REQUESTDATACLASS_HPP
