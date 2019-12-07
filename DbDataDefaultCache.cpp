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

}

mccmnc DbDataDefaultCache::GetDefaultRangeCache(std::string phone)
{
    DefaultDataContainer *working_container = working_ptr.load();
    return working_container->get(phone);
}

void DbDataDefaultCache::GetDefaultTables()
{
    DbDefaultTables.clear();

    Connection_T con = nullptr;
    while (con == nullptr) con = ConnectionPool_getConnection(pool);

    TRY
    {
        // looking dafault data
        ResultSet_T r_data = Connection_executeQuery(con, "%s", "SHOW TABLES like '%default'");
        const char *ret_data;
        while (ResultSet_next(r_data))
        {
             ret_data = ResultSet_getString(r_data, 1);
             if (ret_data != NULL)
                 DbDefaultTables.push_back( ret_data  );
             else
                 LOG(WARNING) << "Failed to SHOW TABLES like '%default', old data used ";
        }

    }
    CATCH(SQLException)
    {
        LOG(WARNING) << "Failed to SHOW TABLES like '%default', old data used " <<  Exception_frame.message;
    }
    FINALLY
    {
        Connection_close(con);
    }
    END_TRY;


}

std::vector<defaultDataRecord> DbDataDefaultCache::GetDefaultTableRecords(std::string tablename)
{

    std::vector<defaultDataRecord> defaultDataRecords;
    std::string sql_default;

    Connection_T con = nullptr;
    while (con == nullptr) con = ConnectionPool_getConnection(pool);

    TRY
    {
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
    }
    CATCH(SQLException)
    {
        LOG(WARNING) << "DbDataDefaultCache::GetDefaultTableRecords Failed " << sql_default <<  Exception_frame.message;
        throw std::runtime_error(Exception_frame.message);
    }
    FINALLY
    {
        Connection_close(con);
    }
    END_TRY;

    return defaultDataRecords;

}



void DbDataDefaultCache::UpdateDefaultRangeCache()
{
    GetDefaultTables();


    if (DbDefaultTables.size() > 0 )   // Sucseed Get Tables from DB
    try{

        // Clean Second buffer
        if ( working_ptr == &First)
            Second.clean();
        else
            First.clean();

        // Fill second buffer of new data
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

        LOG(INFO) << "DB cache for default update done" ;
    }
    catch(...)
    {
        LOG(WARNING) << "DB cache for default update finished with errors ... leaved previous cache";
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
