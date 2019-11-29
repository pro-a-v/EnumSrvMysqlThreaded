#include "DnsMessage.hpp"

DnsMessage::DnsMessage(char *data, int len)
{
    try {
        if (len > 11)  // 12 byte header of DNS record
        {
            header.ID[0] = data[0];
            header.ID[1] = data[1];
            header.QR = data[2] & 128;
            header.OPCODE = (data[2] & 120) >> 3; // must be 0 for QUERY
            header.AA = data[2] & 4;   // Authoritative Answer Flag
            header.TC = data[2] & 2;   // Truncation Flag
            header.RD = data[2] & 1;   // Recursion Desired
            header.RA = data[3] & 128;  // Recursion Avalible
            header.Z = ( data[3] & 112 ) >> 3;
            header.RCODE =  data[3] & 15;
            header.qdc = uint16_value(data + 4);
            header.adc = uint16_value(data + 6);
            header.nsc = uint16_value(data + 8);
            header.arc = uint16_value(data + 10);

            if (header.QR!=false)
            {
                Error=true;  // we serve only questions
                ErrorText = "Income request is not a Query";
            }

            if (header.qdc>1)
            {
                Error=true;    // only one question is allowed
                ErrorText = "Only one question is allowed";
            }

            // Get Request
            if (Error == false)
            {
                req.size = len - 12;
                std::memcpy(req.data , data + 12, req.size );
                req.Type = uint16_value(req.data + req.size - 4 );
                req.Class = uint16_value(req.data + req.size - 2 );
            }

            // We serve only NAPTR
            if (req.Type != 35)
            {
                Error=true;    // only one question is allowed
                ErrorText = "We serve only NAPTR, got " + std::to_string(req.Type);
            }

            // Class only IN
            if (req.Class != 1)
            {
                Error=true;    // only one question is allowed
                ErrorText = "We serve only NAPTR with Class = IN, got Class " + std::to_string(req.Class);
            }

            req.number = NumberFromStr(req.data, req.size);
            //std::cout << std::string("Number ") << req.number << std::string(" requested") << std::endl;
        }
        else
        {
            Error=true;    // only one question is allowed
            ErrorText = "Bad lenth ";
        }

    }
    catch(const std::exception& ex)
    {
        std::cerr << "Error occurred: " << ex.what() << std::endl;

    }
    catch (...) {
        Error=true;    // only one question is allowed
        ErrorText = "got Exception while Compile answer ";
    }

}


std::string DnsMessage::GetRequestedNumber()
{
   return req.number;
}

std::string DnsMessage::AnswerError()
{
    std::string msg;
    char ch;

    try {
        msg.push_back(header.ID[0]);
        msg.push_back(header.ID[1]);
      header.QR = true;
      if (header.QR) ch = 128; else ch = 0;
      ch += header.OPCODE << 3;
      if (header.AA) ch += 4;
      if (header.TC) ch += 2;
      if (header.RD) ch++;
      msg.append(&ch, 1);
      if (header.RA) ch = 128; else ch = 0;
      ch += header.Z << 4;
      header.RCODE = 2; //
      //      RCODE:0	 DNS Query completed successfully
      //      RCODE:1	 DNS Query Format Error
      //      RCODE:2	 Server failed to complete the DNS request
      //      RCODE:3	 Domain name does not exist.  For help resolving this error, read here.
      //      RCODE:4	 Function not implemented
      //      RCODE:5	 The server refused to answer for the query
      //      RCODE:6	 Name that should not exist, does exist
      //      RCODE:7	 RRset that should not exist, does exist
      //      RCODE:8	 Server not authoritative for the zone
      //      RCODE:9	  Name not in zone

      ch += header.RCODE;
      msg.append(&ch, 1);
      msg.append(uint16_buff(0), 2);
      msg.append(uint16_buff(0), 2);
      msg.append(uint16_buff(0), 2);
      msg.append(uint16_buff(0), 2);

      /* write number of written items */


    }
    catch(const std::exception& ex)
    {
        std::cerr << "Error occurred: " << ex.what() << std::endl;

    }
    catch (...)
    {
        Error=true;    // only one question is allowed
        ErrorText = "got Exception while Compile answer ";
        return "";
    }

    return msg;

}

std::string DnsMessage::AnswerAccessDeny()
{
    std::string msg;
    char ch;

    try {
        msg.push_back(header.ID[0]);
        msg.push_back(header.ID[1]);
      header.QR = true;
      if (header.QR) ch = 128; else ch = 0;
      ch += header.OPCODE << 3;
      if (header.AA) ch += 4;
      if (header.TC) ch += 2;
      if (header.RD) ch++;
      msg.append(&ch, 1);
      if (header.RA) ch = 128; else ch = 0;
      ch += header.Z << 4;
      header.RCODE = 5; //
      //      RCODE:0	 DNS Query completed successfully
      //      RCODE:1	 DNS Query Format Error
      //      RCODE:2	 Server failed to complete the DNS request
      //      RCODE:3	 Domain name does not exist.  For help resolving this error, read here.
      //      RCODE:4	 Function not implemented
      //      RCODE:5	 The server refused to answer for the query
      //      RCODE:6	 Name that should not exist, does exist
      //      RCODE:7	 RRset that should not exist, does exist
      //      RCODE:8	 Server not authoritative for the zone
      //      RCODE:9	  Name not in zone

      ch += header.RCODE;
      msg.append(&ch, 1);
      msg.append(uint16_buff(0), 2);
      msg.append(uint16_buff(0), 2);
      msg.append(uint16_buff(0), 2);
      msg.append(uint16_buff(0), 2);

      /* write number of written items */


    }
    catch(const std::exception& ex)
    {
        std::cerr << "Error occurred: " << ex.what() << std::endl;

    }
    catch (...)
    {
        Error=true;    // only one question is allowed
        ErrorText = "got Exception while Compile answer ";
        return "";
    }

    return msg;
}


std::string DnsMessage::Answer(uint16_t mcc,uint8_t mnc)
{
    std::string msg;
    char ch;

    try {
      msg.push_back(header.ID[0]);
      msg.push_back(header.ID[1]);
      header.QR = true;
      if (header.QR) ch = 128; else ch = 0;
      ch += header.OPCODE << 3;
      if (header.AA) ch += 4;
      if (header.TC) ch += 2;
      if (header.RD) ch++;
      msg.append(&ch, 1);
      if (header.RA) ch = 128; else ch = 0;
      ch += header.Z << 4;
      header.RCODE = 0; //
      //      RCODE:0	 DNS Query completed successfully
      ch += header.RCODE;
      msg.append(&ch, 1);
      msg.append(uint16_buff(header.qdc), 2);
      header.adc = 1;
      msg.append(uint16_buff(header.adc), 2);
      msg.append(uint16_buff(0), 2);
      msg.append(uint16_buff(0), 2);


      // write questions
      msg.append(req.data, req.size);


      // write answer
      const char NAPTR_ANSWER [] = {
        // Domain points to name from question
        (char)0xc0, 0x0c,
        // NAPTR type
        0x00, 0x23,
        // IN Class
        0x00, 0x01,
        // TTL (12345 by default)
        0x00, 0x00, 0x30, 0x39,
      };

      msg.append(NAPTR_ANSWER,10);

      char a = 0x00;
      uint16_t data_length=0;

      std::string tmp_data;

      // order
      uint16_t order=10;
      tmp_data.append(uint16_buff(order), 2);
      // preference
      uint16_t preference=50;
      tmp_data.append(uint16_buff(preference), 2);
      // Flags Length
      a = 0x01;
      tmp_data.push_back(a);
      // Flags
      a = 'u';
      tmp_data.push_back(a);
      // Service Length
      a = 0x0c;
      tmp_data.push_back(a);
      // Service
      tmp_data.append(std::string("E2U+pstn:tel"));
      // Regex
      std::string regex;
      if (mnc<10) regex = std::string("!^(.*)$!tel:\\1;npdi;spn=56175;mcc=") + std::to_string(mcc) + std::string(";mnc=0") + std::to_string(mnc) + std::string("!");
      else        regex = std::string("!^(.*)$!tel:\\1;npdi;spn=56175;mcc=") + std::to_string(mcc) + std::string(";mnc=") + std::to_string(mnc) + std::string("!");

      uint8_t regex_size = regex.size();
      tmp_data.push_back(regex_size);
      tmp_data.append(regex);

      // add to msg
      uint16_t tmp_data_size = tmp_data.size() + 1;
      msg.append(uint16_buff(tmp_data_size), 2);
      msg.append(tmp_data);
      msg.push_back(0x00);

    }
    catch(const std::exception& ex)
    {
        std::cerr << "Error occurred: " << ex.what() << std::endl;

    }
    catch (...)
    {
        Error=true;    // only one question is allowed
        ErrorText = "got Exception while Compile answer ";
        return "";
    }

    return msg;

}


std::string DnsMessage::Answer(std::string mcc,std::string mnc)
{
    std::string msg;
    char ch;

    try {
      msg.push_back(header.ID[0]);
      msg.push_back(header.ID[1]);
      header.QR = true;
      if (header.QR) ch = 128; else ch = 0;
      ch += header.OPCODE << 3;
      if (header.AA) ch += 4;
      if (header.TC) ch += 2;
      if (header.RD) ch++;
      msg.append(&ch, 1);
      if (header.RA) ch = 128; else ch = 0;
      ch += header.Z << 4;
      header.RCODE = 0; //
      //      RCODE:0	 DNS Query completed successfully
      ch += header.RCODE;
      msg.append(&ch, 1);
      msg.append(uint16_buff(header.qdc), 2);
      header.adc = 1;
      msg.append(uint16_buff(header.adc), 2);
      msg.append(uint16_buff(0), 2);
      msg.append(uint16_buff(0), 2);


      // write questions
      msg.append(req.data, req.size);


      // write answer
      const char NAPTR_ANSWER [] = {
        // Domain points to name from question
        (char)0xc0, 0x0c,
        // NAPTR type
        0x00, 0x23,
        // IN Class
        0x00, 0x01,
        // TTL (12345 by default)
        0x00, 0x00, 0x30, 0x39,
      };

      msg.append(NAPTR_ANSWER,10);

      char a = 0x00;
      uint16_t data_length=0;

      std::string tmp_data;

      // order
      uint16_t order=10;
      tmp_data.append(uint16_buff(order), 2);
      // preference
      uint16_t preference=50;
      tmp_data.append(uint16_buff(preference), 2);
      // Flags Length
      a = 0x01;
      tmp_data.push_back(a);
      // Flags
      a = 'u';
      tmp_data.push_back(a);
      // Service Length
      a = 0x0c;
      tmp_data.push_back(a);
      // Service
      tmp_data.append(std::string("E2U+pstn:tel"));
      // Regex
      std::string regex;
      regex = std::string("!^(.*)$!tel:\\1;npdi;spn=56175;mcc=") + mcc + std::string(";mnc=") + mnc + std::string("!");

      uint8_t regex_size = regex.size();
      tmp_data.push_back(regex_size);
      tmp_data.append(regex);

      // add to msg
      uint16_t tmp_data_size = tmp_data.size() + 1;
      msg.append(uint16_buff(tmp_data_size), 2);
      msg.append(tmp_data);
      msg.push_back(0x00);

    }
    catch(const std::exception& ex)
    {
        std::cerr << "Error occurred: " << ex.what() << std::endl;

    }
    catch (...)
    {
        Error=true;    // only one question is allowed
        ErrorText = "got Exception while Compile answer ";
        return "";
    }

    return msg;

}


uint16_t DnsMessage::uint16_value(char *buff) {
  return buff[0] * 256 + buff[1];
}

uint32_t DnsMessage::uint32_value(char *buff) {
  return uint16_value(buff) * 65536 + uint16_value(buff + 2);
}

char* DnsMessage::uint16_buff(uint16_t val) {
  _tmp[0] = val / 256;
  _tmp[1] = val & 255;
  _tmp[2]=0x00;
  return _tmp;
}

char* DnsMessage::uint32_buff(uint32_t val) {
  _tmp[0] = val / (1 << 24);
  _tmp[1] = val / (1 << 16);
  _tmp[2] = val / (1 << 8);
  _tmp[3] = val;
  _tmp[4]=0x00;
  return _tmp;
}

uint64_t DnsMessage::to_uint64(std::string in) {
  static_assert(sizeof(unsigned long long) == sizeof(uint64_t), "ull not large enough");
  unsigned long long val = 0;
  std::stringstream stream(in);
  stream >> val;
  return val;
}

std::string DnsMessage::NumberFromStr(char *data, int len)
{
    std::string value;
    try {
        // RFC 4034  The RDATA section of the NSEC RR above would be encoded as:
         int length = 24;
         char *pos = strchr(data,0x65); //  e164 serach for 'e'
         if (pos != NULL)
         {
           length = pos-data-1;
         }

         int simbols_count = 0;



         if (len<length)
         {
             Error=true;    // only one question is allowed
             ErrorText = "wrong telephone number: " + std::string(data,len);
             return 0;
         }


         for(int i=0; i<=length;i++)   // data contain Class and Type
         {
             char ch;
             ch = data[i];

             if (simbols_count==2)
             {
                 std::reverse(value.begin(),value.end());
                 return value;
             }

             if ( (0x30<=ch) && (ch<=0x39) )
              {
                 simbols_count=0;
                 value.append(data + i,1);
             }
             else
             {
                simbols_count++;
             }
         }

        std::reverse(value.begin(),value.end());
    }
    catch(const std::exception& ex)
    {
        std::cerr << "Error occurred: " << ex.what() << std::endl;
    }


return value;
}
