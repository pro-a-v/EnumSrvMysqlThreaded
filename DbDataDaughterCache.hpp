#ifndef DBDATADAUGHTERCACHE_HPP
#define DBDATADAUGHTERCACHE_HPP

#include "daughter_range_cache_data.hpp"
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
    std::vector<daughterDataRecord> *GetDaughterTableRecords(std::string tablename);
    std::vector<std::string> DbDaughterTables;

    void UpdateDaughterCache();
    void UpdateDaughterRangeCache();
    void UpdateDaughterRangeCacheWorker();
    std::mutex daughterDataMutex;
    bool condition_working;
    std::vector<daughterDataRecord> *daughterDataRecords;
    std::map<std::string, void*> daughterDataContainer;
    std::map<std::string, void*>::iterator daughterDataContainerIterator;
    std::vector<daughterDataRecord>::iterator daughterDataRecordsIterator;

    // ----------------------

    boost::asio::io_service *io_service;

};

#endif // DBDATADAUGHTERCACHE_HPP
