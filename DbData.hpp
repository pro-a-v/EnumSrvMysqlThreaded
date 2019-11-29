#ifndef DBDATA_HPP
#define DBDATA_HPP

#include <iostream>
#include <string.h>
#include <cstring>

#include "mccmnc.hpp"
#include "default_range_cache_data.hpp"
#include "DbDataDefaultCache.hpp"
#include "DbDataDaughterCache.hpp"

#include <stdexcept>
#include <vector>
#include <mutex>

#include <zdb/zdb.h>
#include <zdb/SQLException.h>

#include <g3log/g3log.hpp>

class DbData
{
public:
    DbData(ConnectionPool_T pool_);
    ~DbData();
    mccmnc get(std::string phone_, DbDataDefaultCache *DbDataDefaultCache, DbDataDaughterCache *DbDataDaughterCache);
    bool isPortable();
    void GetParent();
    void GetDefaultRange();




    std::string currentDateTime();

private:

    ConnectionPool_T pool;
    Connection_T con = nullptr;

    std::string phone;
    std::string prefix;
    std::string table_template;
    std::string table_data;
    std::string table_default;
    std::string table_daughter;

    mccmnc mcc_data;



};

#endif // DBDATA_HPP
