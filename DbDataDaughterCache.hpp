#ifndef DBDATADAUGHTERCACHE_HPP
#define DBDATADAUGHTERCACHE_HPP

#include "daughter_range_cache_data.hpp"
#include "mccmnc.hpp"
#include "DaugterCacheContainer.hpp"

#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>

#include <atomic>
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

#include <g3log/g3log.hpp>

class DbDataDaughterCache
{
public:
    DbDataDaughterCache(boost::asio::io_service *io_service, ConnectionPool_T pool_);
    ~DbDataDaughterCache();

    mccmnc GetDaughterRangeCache(std::string phone, mccmnc origin);
private:

    ConnectionPool_T pool;

    // Default Ranges Cache
    void GetDaughterTables();
    std::vector<std::string> DbDaughterTables;

    void GetDaughterTableRecords(std::string tablename);
    std::vector<daughterDataRecord> daughterDataRecords;


    void UpdateDaughterCache();
    void UpdateDaughterRangeCache();
    void UpdateDaughterRangeCacheWorker();

    bool condition_working;


    DaugterCacheContainer First, Second;
    std::atomic<DaugterCacheContainer*> working_ptr;

    // ----------------------

    boost::asio::io_service *io_service;

};

#endif // DBDATADAUGHTERCACHE_HPP
