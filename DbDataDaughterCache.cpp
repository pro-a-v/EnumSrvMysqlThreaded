#include "DbDataDaughterCache.hpp"

DbDataDaughterCache::DbDataDaughterCache(boost::asio::io_service *io_service_, ConnectionPool_T pool_): io_service(io_service_), pool(pool_)
{
    UpdateDaughterRangeCache();
    condition_working = true;
    boost::thread *UpdaterThread = new boost::thread(   boost::bind(&DbDataDaughterCache::UpdateDaughterRangeCacheWorker, this) , nullptr );

}

DbDataDaughterCache::~DbDataDaughterCache()
{
    ;
}

mccmnc DbDataDaughterCache::GetDaughterRangeCache(std::string phone, mccmnc origin)
{
    std::lock_guard<std::mutex> guard(daughterDataMutex);
    std::string prefix;
    mccmnc tmp = origin;
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
    std::string table_daughter = table_template + std::string("daughter");

    try {
        daughterDataRecords = (std::vector<daughterDataRecord> *)daughterDataContainer[table_daughter];


        for (daughterDataRecordsIterator=daughterDataRecords->begin();daughterDataRecordsIterator!=daughterDataRecords->end();daughterDataRecordsIterator++)
        {
            daughterDataRecord tmp_data = (*daughterDataRecordsIterator);
            if (tmp_data.db_mnc == origin.mnc )
            if (tmp_data.db_mcc == origin.mcc )
            {
                tmp.mcc = tmp_data.db_parent_mcc;
                tmp.mnc = tmp_data.db_parent_mnc;

            }
        }

    } catch (...) {;}



    return tmp;
}

void DbDataDaughterCache::GetDaughterTables()
{
    std::string sql_default = "SHOW TABLES like '%daughter'";
    Connection_T con = ConnectionPool_getConnection(pool);
    TRY
        // looking dafault data

        ResultSet_T r_data = Connection_executeQuery(con, "%s", sql_default.c_str());

        int rows = Connection_getFetchSize(con);
        if ( rows > 0)     DbDaughterTables.clear();

        while (ResultSet_next(r_data))
        {
             DbDaughterTables.push_back(  ResultSet_getString(r_data, 1) );
        }

    CATCH(SQLException)
        std::cout << "Failed: " << sql_default <<  Exception_frame.message;
        //throw std::runtime_error("SHOW TABLES like '%daughter'");
    END_TRY;
    Connection_close(con);

}

std::vector<daughterDataRecord> *DbDataDaughterCache::GetDaughterTableRecords(std::string tablename)
{
    std::vector<daughterDataRecord> *daughterDataRecords = new std::vector<daughterDataRecord>;
    std::string sql_default = "select  `mcc`, `mnc`, `parent_mcc`, `parent_mnc` from " + tablename;
    Connection_T con = ConnectionPool_getConnection(pool);

    TRY
        // looking dafault data

        ResultSet_T r_data = Connection_executeQuery(con, "%s", sql_default.c_str());

        while (ResultSet_next(r_data))
        {
             daughterDataRecord tmp_data;

             tmp_data.db_mcc = ResultSet_getIntByName(r_data, "mcc");
             tmp_data.db_mnc = ResultSet_getIntByName(r_data, "mnc");
             tmp_data.db_parent_mcc = ResultSet_getIntByName(r_data, "parent_mcc");
             tmp_data.db_parent_mnc = ResultSet_getIntByName(r_data, "parent_mnc");
             daughterDataRecords->push_back(tmp_data);
        }

    CATCH(SQLException)
        std::string sql_default = "SHOW TABLES like '%daughter'";
        //throw std::runtime_error(sql_default);
    END_TRY;
    Connection_close(con);
    return daughterDataRecords;

}



void DbDataDaughterCache::UpdateDaughterRangeCache()
{
    GetDaughterTables();

    std::lock_guard<std::mutex> guard(daughterDataMutex);
    // Clean Current values
    for(auto &daughterDataContainerItem : daughterDataContainer)
    {
           delete  (std::vector<daughterDataRecord>*)daughterDataContainerItem.second;
    }
    daughterDataContainer.clear();



    std::vector<std::string>::const_iterator table;
    for(auto table : DbDaughterTables)
    {
            std::vector<daughterDataRecord> *daughterDataRecords = GetDaughterTableRecords(table);
            daughterDataContainer.insert(std::make_pair(table, (void*)daughterDataRecords));
    }
    std::cout << "DB cache for daughter update done" << std::endl;
}

void DbDataDaughterCache::UpdateDaughterRangeCacheWorker()
{
    while(condition_working)
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(300000)); // 5 min
        UpdateDaughterRangeCache();
    }
}
