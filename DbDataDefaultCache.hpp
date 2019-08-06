#ifndef DBDATADEFAULTCACHE_HPP
#define DBDATADEFAULTCACHE_HPP

#include "default_range_cache_data.hpp"
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
#include <boost/ptr_container/ptr_map.hpp>

#include <zdb/zdb.h>
#include <zdb/SQLException.h>

class DbDataDefaultCache
{
public:
    DbDataDefaultCache(boost::asio::io_service *io_service, ConnectionPool_T pool_);
    ~DbDataDefaultCache();

    mccmnc GetDefaultRangeCache(std::string phone);
private:

    ConnectionPool_T pool;

    // Default Ranges Cache
    void GetDefaultTables();
    std::vector<defaultDataRecord> GetDefaultTableRecords(std::string tablename);
     std::vector<std::string> DbDefaultTables;

    void UpdateDefaultCache();
    void UpdateDefaultRangeCache();
    void UpdateDefaultRangeCacheWorker();
    std::mutex defaultDataMutex;
    bool condition_working;
    std::vector<defaultDataRecord> defaultDataRecords;
    std::map<std::string, std::vector<defaultDataRecord>> defaultDataContainer;


    // ----------------------

    boost::asio::io_service *io_service;

};

#endif // DBDATADEFAULTCACHE_HPP
