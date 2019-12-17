#include "Hlr_Requests_HTTP11_Pipelined_Client.hpp"


using boost::asio::ip::tcp;

Hlr_Requests_HTTP11_Pipelined_Client::Hlr_Requests_HTTP11_Pipelined_Client(boost::asio::io_service& io_service, size_t requests_count,  BlockingQueue<Request *> *income_queue, boost::asio::ip::udp::socket *socket_udp_):
    resolver_(io_service), socket_(io_service), income_queue_(income_queue), socket_udp(socket_udp_), requests_count_(requests_count)
{
    for (size_t i=0; i < requests_count; i++)
    {
          requests.push_back(income_queue->pop());
    }

    LOG(WARNING) << "Thread: " << this << " Total requests - " << requests_count_;
    client_start_utc_time = boost::posix_time::microsec_clock::universal_time();

    create_http_requests();

    // Attempt a connection to endpoint
    //boost::asio::ip::tcp::endpoint tcp_endpoint( boost::asio::ip::address::from_string("10.15.0.81"),  15335   );
    boost::asio::ip::tcp::endpoint tcp_endpoint( boost::asio::ip::address::from_string("127.0.0.1"),  15335   );
    socket_.async_connect(tcp_endpoint,  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_connect, this,  boost::asio::placeholders::error));
}

Hlr_Requests_HTTP11_Pipelined_Client::~Hlr_Requests_HTTP11_Pipelined_Client()
{
    LOG(WARNING) << "Thread: " << this << " destroyed ";
}

void Hlr_Requests_HTTP11_Pipelined_Client::create_http_requests()
{

    std::ostream request_stream(&request_);
    for (auto const &Req: requests)
    {
    // Form the request.


    request_stream << "POST /UssdService.svc HTTP/1.1\r\n";
    request_stream << "Host: 10.15.0.81:15335\r\n";
    request_stream << "Connection: Keep-Alive\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Type: application/json\r\n";

    std::string rest = std::string("{\"srism_req\" : {\"msisdn\":\"") + Req->phone_number + std::string("\",\"request_id\":") + std::to_string(Req->uid) + std::string("}}\r\n");

    request_stream << "Content-Length: "<< rest.size() <<"\r\n\r\n";
    request_stream << rest;
    }
}

void Hlr_Requests_HTTP11_Pipelined_Client::handle_connect(const boost::system::error_code& err)
  {
    if (!err)
    {
      // The connection was successful. Send the request.
      boost::asio::async_write(socket_, request_,  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_write_request, this, boost::asio::placeholders::error));
    }
    else
    {
      LOG(WARNING) << "Error: " << err.message();
    }
  }

void Hlr_Requests_HTTP11_Pipelined_Client::handle_write_request(const boost::system::error_code& err)
  {
    if (!err)
    {
      // Read the response
      boost::asio::async_read_until(socket_, response_, "\r\n\r\n",  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce_headers, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ));
    }
    else
    {
      LOG(WARNING) << "Error: " << err.message();
    }
  }





void Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce_headers(const boost::system::error_code& err, std::size_t bytes_transferred)
{
    if (!err)
    {
      LOG(WARNING) << " Responce size = " << response_.size();


      std::istream response_stream(&response_);
      std::string header, headers;
      std::string content_length = std::string("content-length:");
      std::string content_length_value = "0";
      body_bytes_size = 0;

      //read the headers.
      while (std::getline(response_stream, header) && header != "\r")
      {
            headers.append(header);
            boost::algorithm::to_lower(header);
            std::size_t found = header.find(content_length);
            if ( found != std::string::npos)
            {
                  content_length_value =  header.substr(found + content_length.size() ) ;
                  boost::trim(content_length_value);
                  body_bytes_size = boost::lexical_cast<std::size_t>(content_length_value);
            }
      }

      if ( body_bytes_size > 0 )
      {
          // read body
          LOG(WARNING) << "body_bytes_size: " << body_bytes_size << " response_.size():" << response_.size();

          if (body_bytes_size <= response_.size())
          {

              handle_read_responce_body(err, body_bytes_size);
          }
          else
          {
            std::size_t need_to_read = body_bytes_size - response_.size();
            boost::asio::async_read(socket_, response_, boost::asio::transfer_at_least(need_to_read),  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce_body, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ));
          }
      }
      else
      {
          LOG(WARNING) << "No content length in responce " << headers;
      }


    }
    else if (err != boost::asio::error::eof)
    {
      LOG(WARNING) << "Error: " << err;
      delete this;
    }
    else if (err == boost::asio::error::eof)
    {
        LOG(WARNING) << "Error: connection closed " << err;
    }
}

void Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce_body(const boost::system::error_code &err, std::size_t bytes_transferred)
{
    if (!err)
    {
        std::ostringstream ss;
        try {
            std::istream response_stream(&response_);


            char data[1024];  memset(data, 0x00, 1024);
            response_stream.read(data, body_bytes_size);
            //LOG(INFO) << data;

            rapidjson::Document doc;
            doc.Parse(data);
            rapidjson::Value& srism_ack = doc["srism_ack"];
            std::string mcc_mnc = srism_ack["mccmnc"].GetString();
            unsigned int uid = srism_ack["request_id"].GetInt();
            std::string error_code  = srism_ack["errcode"].GetString();
            process_answer(error_code, uid, mcc_mnc);

            if (response_.size() > 200) // We have additional data in buffer
            {
                LOG(WARNING) << " Have else data in buffer - process";
                handle_read_responce_headers(err, response_.size());

            }
            else if ( requests_count_ > 0 ) // Not all requests processed - read socket
            {
                LOG(WARNING) << "Thread: " << this << " Read other answer. To read - " << requests.size();
                boost::asio::async_read_until(socket_, response_, "\r\n\r\n",  boost::bind(&Hlr_Requests_HTTP11_Pipelined_Client::handle_read_responce_headers, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ));
            }
            else
            {
                delete this;
            }


        } catch (...) {}


    }
    else if (err != boost::asio::error::eof)
    {
      LOG(WARNING) << "Error: " << err;
      delete this;
    }
    else if (err == boost::asio::error::eof)
    {
        LOG(WARNING) << "Error: Connection closed";
        delete this;
    }
    
}

void Hlr_Requests_HTTP11_Pipelined_Client::process_answer(std::string error_code, unsigned int uid, std::string mcc_mnc)
{
 // error_code - Possible values : - Success - Unknown Subscriber - Absent Subscriber - System failure - Data missing - Unexpected Data Value - Illegal Equipment - Timeout
    DnsMessage NS;
    Request *req = get_req(uid);
    requests_count_ -=1;

    if (req == nullptr)
    {
        LOG(WARNING) << "Not found request with uid: " << uid;
    }


    if (error_code == std::string("Success"))
    {
        if (req != nullptr)
        {
        NS.parse(const_cast<char*>(req->raw_data.c_str()),req->raw_data.size());
        boost::system::error_code ignored_ec;
        socket_udp->send_to(boost::asio::buffer(NS.Answer(mcc_mnc.substr(0,3),mcc_mnc.substr(3,mcc_mnc.size()-3))), req->sender_endpoint_, 0, ignored_ec);
        delete req;
        }

    }
    else
    {
        if (req != nullptr)
        {
        NS.parse(const_cast<char*>(req->raw_data.c_str()),req->raw_data.size());
        boost::system::error_code ignored_ec;
        socket_udp->send_to(boost::asio::buffer(NS.Answer(req->default_mccmnc.mcc,req->default_mccmnc.mnc)), req->sender_endpoint_, 0, ignored_ec);
        delete req;
        }
    }
}

Request *Hlr_Requests_HTTP11_Pipelined_Client::get_req(unsigned int uid)
{
    for (std::vector<Request *>::iterator it = requests.begin() ; it != requests.end(); ++it)
    {
        if ( (*it)->uid == uid )
        {
            Request *req = (*it);
            requests.erase(it);
            return req;
        }
    }
    return nullptr;
}
