#include "IOServer.h"



IOServer::IOServer(boost::asio::io_service& io_service, short port)
    : HlrCounter(), socket_(io_service, udp::endpoint(udp::v4(), port))
{

    done.store(false, boost::memory_order_release);

    ZBDEBUG = false;
    Exception_init();

    // Myslq
    url = URL_new(testURL);
    pool = ConnectionPool_new(url);
    assert(pool);
    ConnectionPool_setInitialConnections(pool,4);
    ConnectionPool_setReaper(pool, 60);
    ConnectionPool_setMaxConnections(pool,1000);
    ConnectionPool_start(pool);



    // worker threads
    //unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
    //if (concurentThreadsSupported == 0)
    //    concurentThreadsSupported = 1;


    DefaultDataCache = new DbDataDefaultCache(&io_service, pool);
    DaughterDataCache = new DbDataDaughterCache(&io_service, pool);
    ClientsDenyList = new DbDataClientsDenyList(&io_service, pool);




    for (unsigned i=0;i<16;i++)
    {
        boost::thread worker(&IOServer::RequestConsumerWorker, this);
        boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    }



    socket_.async_receive_from( boost::asio::buffer(data_, max_length), sender_endpoint_, [this](boost::system::error_code ec, std::size_t bytes_recvd)
        {
          if (!ec && bytes_recvd > 0)
          {
             Request *request = new Request(data_,bytes_recvd, sender_endpoint_);
             income_queue.push(request);
          }
    	    boost::this_thread::sleep_for(boost::chrono::milliseconds(2));
            do_receive();

    });
}

IOServer::~IOServer()
{
   done.store(true, boost::memory_order_release);

   // Free Mysql
  ConnectionPool_stop(pool);
  ConnectionPool_free(&pool);
  assert(pool==NULL);
  URL_free(&url);

}

void IOServer::do_receive()
{
  socket_.async_receive_from( boost::asio::buffer(data_, max_length), sender_endpoint_, [this](boost::system::error_code ec, std::size_t bytes_recvd)
      {
        if (!ec && bytes_recvd > 0)
        {
          Request *request = new Request(data_,bytes_recvd,sender_endpoint_);
          income_queue.push(request);
        }
          boost::this_thread::sleep_for(boost::chrono::milliseconds(2));
          do_receive();

      });
}


std::string IOServer::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

void IOServer::RequestConsumerWorker()
{
    DbData dbd = DbData(pool);

    while(! done.load(boost::memory_order_relaxed))
    {
        //boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        Request *req = nullptr;
        req = income_queue.pop();
        if (req != nullptr)
        {
            // обрабатываем запрос
            DnsMessage NS;
            try
            {
                NS.parse(const_cast<char*>(req->raw_data.c_str()),req->raw_data.size());

                if (NS.Error)
                {
                    SendErrorAnswer(&NS , req);
                    delete req;
                    continue;
                }

            }
            catch(const std::exception& ex)
            {
                LOG(WARNING) << "ClientsDenyList->isAlowed: Error occurred: " << ex.what();
            }





            // If NOT Allowed send AccessDeny and continue with new req
            try
            {
                if ( ! ClientsDenyList->isAlowed(req->sender_endpoint_.address().to_v4().to_string(), NS.GetRequestedNumber()) )
                {
                    SendAccessDenyAnswer(&NS , req);
                    delete req;
                    continue;
                }
            }
            catch(const std::exception& ex)
            {
                LOG(WARNING) << "ClientsDenyList->isAlowed: Error occurred: " << ex.what();
            }



            if (ProcessDBRequest(dbd, &NS, req))
            {
                delete req;
                continue;
            }
            else
            {
                // error answer
                SendErrorAnswer(&NS , req);
                delete req;
            }




        }

    }
}

void IOServer::SendErrorAnswer(DnsMessage *NS ,Request *req)
{
    try{
    boost::system::error_code ignored_ec;
    this->socket_.send_to(boost::asio::buffer(NS->AnswerError()), req->sender_endpoint_, 0, ignored_ec);
    }
    catch(const std::exception& ex)
    {
        LOG(WARNING) << "IOServer::SendErrorAnswer: Error occurred: " << ex.what();
    }
}

void IOServer::SendAccessDenyAnswer(DnsMessage *NS ,Request *req)
{
    try {
        boost::system::error_code ignored_ec;
        this->socket_.send_to(boost::asio::buffer(NS->AnswerAccessDeny()), req->sender_endpoint_, 0, ignored_ec);
    }
    catch(const std::exception& ex)
    {
        LOG(WARNING) << "IOServer::SendErrorAnswer: Error occurred: " << ex.what();
    }
}

bool IOServer::ProcessDBRequest(DbData &dbd, DnsMessage *NS ,Request *req)
{
    try
    {
            mccmnc mcc_data = dbd.get(NS->GetRequestedNumber(),  DefaultDataCache, DaughterDataCache);
            boost::system::error_code ignored_ec;
            this->socket_.send_to(boost::asio::buffer(NS->Answer(mcc_data.mcc,mcc_data.mnc)), req->sender_endpoint_, 0, ignored_ec);
            return true;
    }
    catch(const std::exception& ex)
    {
        LOG(WARNING) << "IOServer::ProcessDBRequest Error occurred: " << ex.what();
    }

    return false;
}
