# include "RequestClass.hpp"

Request::Request(char *data, int len, boost::asio::ip::udp::endpoint sender_endpoint): sender_endpoint_(sender_endpoint)
{
    raw_data = std::string(data, len);
    income_utc_time =  boost::posix_time::microsec_clock::universal_time();
}

