#ifndef DBDATACLIENTDENYLIST_HPP
#define DBDATACLIENTDENYLIST_HPP

#include "mccmnc.hpp"

#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>

#include <iostream>
#include <string.h>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <mutex>
#include <map>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <zdb/zdb.h>
#include <zdb/SQLException.h>

class DbDataClientsDenyList
{
public:
    DbDataClientsDenyList(boost::asio::io_service *io_service, ConnectionPool_T pool_);
    ~DbDataClientsDenyList();

    bool isAlowed(std::string ip, std::string phone);
private:

    ConnectionPool_T pool;

    void UpdateClientsDenyList();
    void UpdateClientsDenyListWorker();

    std::mutex ClientsDenyListDataMutex;
    bool condition_working;

    std::map<std::string, std::string> ClientsDenyListDataContainer;
    std::map<std::string, std::string>::iterator ClientsDenyListDataContainerIterator;


    // ----------------------

    boost::asio::io_service *io_service;

};

#endif // DBDATACLIENTDENYLIST_HPP
