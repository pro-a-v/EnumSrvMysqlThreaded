#ifndef REQUESTDATACLASS_HPP
#define REQUESTDATACLASS_HPP

#include <boost/asio.hpp>
#include <string.h>
#include <g3log/g3log.hpp>
#include "mccmnc.hpp"


class Request
{
public:
    Request(char *data, int len, boost::asio::ip::udp::endpoint sender_endpoint);

    std::string raw_data;
    boost::asio::ip::udp::endpoint sender_endpoint_;
    boost::posix_time::ptime income_utc_time;
    unsigned int uid; // Uniq Id
    std::string phone_number;
    mccmnc default_mccmnc;
};


#endif // REQUESTDATACLASS_HPP
