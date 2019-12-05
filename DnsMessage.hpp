#ifndef DNSMESSAGE_HPP
#define DNSMESSAGE_HPP

#include  <stdint.h>
#include <string>   // std string
#include <cstring>
#include <algorithm>  // std::reverse(str.begin(), str.end());
#include <sstream>   // stringstream
#include <iostream> // std::cout
#include <boost/atomic.hpp>
#include <g3log/g3log.hpp>

typedef char uint4_t;
typedef char uint3_t;

class DnsMessage
{
private:
    uint16_t uint16_value(char *buff);
    uint32_t uint32_value(char *buff);
    char* uint16_buff(uint16_t val);
    char* uint32_buff(uint32_t val);
    uint64_t to_uint64(std::string in);
    std::string NumberFromStr(char *data, int len);

    char _tmp[5];

    struct header_rfc
    {
        char ID[2];    // Identifier
        bool QR;        // Query/Responce Flag
        uint4_t OPCODE;   // Operation code  0 = query
        bool AA;   // Authoritative Answer Flag
        bool TC;   // Truncation Flag
        bool RD;   // Recursion Desired
        bool RA;  // Recursion Avalible
        uint3_t Z;   // three reserved bits
        uint4_t RCODE;  // Responce code
        uint16_t qdc;  // Questions count
        uint16_t adc;  // Answers Count
        uint16_t nsc;  // Authority count
        uint16_t arc;  // Addition Record Count

    };

    struct query_data
    {
        char data[512];
        int size;
        uint16_t Type;
        uint16_t Class;
        std::string number;
        uint16_t DNS_query_size;
    };

public:
    DnsMessage();
    void parse(char* data, int len);
    header_rfc header;
    query_data req;
    query_data resp;

    bool Error = false;
    std::string ErrorText;

    std::string GetRequestedNumber();
    std::string Answer(uint16_t mcc,uint8_t mnc);
    std::string Answer(std::string mcc,std::string mnc);
    std::string AnswerError();
	std::string AnswerAccessDeny();






};

#endif // DNSMESSAGE_HPP
