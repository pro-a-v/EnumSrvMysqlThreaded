#include "DbDataDefaultCache.hpp"

DbDataDefaultCache::DbDataDefaultCache(boost::asio::io_service *io_service_, ConnectionPool_T pool_): io_service(io_service_), pool(pool_)
{
    UpdateDefaultRangeCache();
    condition_working = true;
    boost::thread *UpdaterThread = new boost::thread(   boost::bind(&DbDataDefaultCache::UpdateDefaultRangeCacheWorker, this) , nullptr );

}

DbDataDefaultCache::~DbDataDefaultCache()
{
    ;
}

mccmnc DbDataDefaultCache::GetDefaultRangeCache(std::string phone)
{
    std::lock_guard<std::mutex> guard(defaultDataMutex);
    std::string prefix;
    mccmnc tmp;
    switch(phone.length())
    {
        case 6: prefix = phone.substr(0,1); break;
        case 7: prefix = phone.substr(0,1); break;
        case 8: prefix = phone.substr(0,1); break;
        case 9: prefix = phone.substr(0,1); break;
        case 10: prefix = phone.substr(0,1); break;
        case 11: prefix = phone.substr(0,2); break;
        case 12: prefix = phone.substr(0,3); break;
        case 13: prefix = phone.substr(0,3); break;
        case 14: prefix = phone.substr(0,3); break;
        case 15: prefix = phone.substr(0,4); break;
        case 16: prefix = phone.substr(0,4); break;
        default: prefix = phone.substr(0,1); break;
    }

    std::string table_template = std::string("l") + std::to_string(phone.length()) + std::string("p") + prefix;
    std::string table_default = table_template + std::string("default");

    try {
        //std::cout << phone << " - " << table_default << std::endl;
        defaultDataRecords = defaultDataContainer[table_default];


        uint64_t int_phone = std::stoull(phone);

        for (auto defaultDataRecord : defaultDataRecords)
        {

            if (defaultDataRecord.db_from <= int_phone )
            if (defaultDataRecord.db_to >= int_phone )
            {
                tmp.mcc = defaultDataRecord.db_mcc;
                tmp.mnc = defaultDataRecord.db_mnc;

            }
        }

    } catch (...) {;}



    return tmp;
}

void DbDataDefaultCache::GetDefaultTables()
{
    Connection_T con = ConnectionPool_getConnection(pool);
    TRY
        // looking dafault data
        std::string sql_default = "SHOW TABLES like '%default'";

        ResultSet_T r_data = Connection_executeQuery(con, "%s", sql_default.c_str());

        int rows = Connection_getFetchSize(con);
        if ( rows > 0)     DbDefaultTables.clear();
        while (ResultSet_next(r_data))
        {
             DbDefaultTables.push_back(  ResultSet_getString(r_data, 1) );
        }

    CATCH(SQLException)
        std::cout << "Failed to SHOW TABLES like '%default', old data used " <<  Exception_frame.message;
        //throw std::runtime_error("SHOW TABLES like '%default' ");
    END_TRY;
    Connection_close(con);

}

std::vector<defaultDataRecord> DbDataDefaultCache::GetDefaultTableRecords(std::string tablename)
{
    std::vector<defaultDataRecord> defaultDataRecords;
    std::string sql_default;
    Connection_T con = ConnectionPool_getConnection(pool);
    TRY
        // looking dafault data
        sql_default = "select `from`, `to`, `mcc`, `mnc` from " + tablename;

        ResultSet_T r_data = Connection_executeQuery(con, "%s", sql_default.c_str());

        while (ResultSet_next(r_data))
        {
             defaultDataRecord tmp_data;
             tmp_data.db_to = ResultSet_getLLongByName(r_data, "to");
             tmp_data.db_from = ResultSet_getLLongByName(r_data, "from");
             tmp_data.db_mcc = ResultSet_getIntByName(r_data, "mcc");
             tmp_data.db_mnc = ResultSet_getIntByName(r_data, "mnc");
             defaultDataRecords.push_back(tmp_data);
        }

    CATCH(SQLException)
        std::cout << "Failed " << sql_default <<  Exception_frame.message;
        //throw std::runtime_error(sql_default);
    END_TRY;
    Connection_close(con);

    return defaultDataRecords;

}



void DbDataDefaultCache::UpdateDefaultRangeCache()
{
    GetDefaultTables();

    std::lock_guard<std::mutex> guard(defaultDataMutex);


    for(auto table : DbDefaultTables)
    {
            defaultDataRecords = GetDefaultTableRecords(table);
            defaultDataContainer[table].clear();
            defaultDataContainer[table].swap(defaultDataRecords);
    }
    std::cout << "DB cache for default update done" << std::endl;
}

void DbDataDefaultCache::UpdateDefaultRangeCacheWorker()
{
    while(condition_working)
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(300000)); // 5 min
        UpdateDefaultRangeCache();

    }
}
