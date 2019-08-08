#include "IOServer.h"


IOServer::IOServer(boost::asio::io_service& io_service, short port)
    : socket_(io_service, udp::endpoint(udp::v4(), port))
{

    done.store(false, boost::memory_order_release);

    ZBDEBUG = false;
    Exception_init();

    // Myslq
    url = URL_new(testURL);
    pool = ConnectionPool_new(url);
    assert(pool);
    ConnectionPool_setInitialConnections(pool,1);
    ConnectionPool_setReaper(pool, 30);
    ConnectionPool_start(pool);
    ConnectionPool_setMaxConnections(pool,64);


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

            DnsMessage NS( const_cast<char*>(req->raw_data.c_str()),req->raw_data.size());
            // формируем ответ
            if (NS.Error==true)
            {
                boost::system::error_code ignored_ec;
                this->socket_.send_to(boost::asio::buffer(NS.AnswerError()), req->sender_endpoint_, 0, ignored_ec);
            }
            else
            {
                std::string phone = NS.GetRequestedNumber();
                try {

                    if ( ClientsDenyList->isAlowed(req->sender_endpoint_.address().to_v4().to_string(),phone) )
                    {
                    mccmnc mcc_data = dbd.get(phone,  DefaultDataCache, DaughterDataCache);
                    boost::system::error_code ignored_ec;
                    this->socket_.send_to(boost::asio::buffer(NS.Answer(mcc_data.mcc,mcc_data.mnc)), req->sender_endpoint_, 0, ignored_ec);
                    continue;
                    }
                } catch (...) {
                    ; // need just log an error
                }
                // error answer
                boost::system::error_code ignored_ec;
                this->socket_.send_to(boost::asio::buffer(NS.AnswerError()), req->sender_endpoint_, 0, ignored_ec);

            }
        delete req;
        }


    }

}


