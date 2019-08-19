#include "DbDataDefaultCache.hpp"

DbDataDefaultCache::DbDataDefaultCache(boost::asio::io_service *io_service_, ConnectionPool_T pool_): io_service(io_service_), pool(pool_)
{
    working_ptr.store(&First);

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
    DefaultDataContainer *working_container = working_ptr;
    return working_container->get(phone);
}

void DbDataDefaultCache::GetDefaultTables()
{
    DbDefaultTables.clear();
    Connection_T con = ConnectionPool_getConnection(pool);
    TRY
        // looking dafault data
        std::string sql_default = "SHOW TABLES like '%default'";

        ResultSet_T r_data = Connection_executeQuery(con, "%s", sql_default.c_str());

        while (ResultSet_next(r_data))
        {
             DbDefaultTables.push_back(  ResultSet_getString(r_data, 1) );
        }

    CATCH(SQLException)
        std::cout << "Failed to SHOW TABLES like '%default', old data used " <<  Exception_frame.message;
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
        throw std::runtime_error(Exception_frame.message);
    END_TRY;
    Connection_close(con);

    return defaultDataRecords;

}



void DbDataDefaultCache::UpdateDefaultRangeCache()
{
    GetDefaultTables();


    if (DbDefaultTables.size() > 0 )   // Sucseed Get Tables from DB
    try{

        for(auto table : DbDefaultTables)
        {
                defaultDataRecords = GetDefaultTableRecords(table);

                if ( working_ptr == &First)
                    Second.update(table, defaultDataRecords);
                else
                    First.update(table, defaultDataRecords);
        }
        // Only without any db error we come here and switch cache
        if ( working_ptr == &First)
            working_ptr.store(&Second);
        else
            working_ptr.store(&First);
        std::cout << "DB cache for default update done" << std::endl;
    }
    catch(...)
    {
        std::cout << "DB cache for default update finished with errors ... leaved previous cache" << std::endl;
    }



}

void DbDataDefaultCache::UpdateDefaultRangeCacheWorker()
{
    while(condition_working)
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(300000)); // 5 min
        UpdateDefaultRangeCache();

    }
}
