#include "DbDataDaughterCache.hpp"

DbDataDaughterCache::DbDataDaughterCache(boost::asio::io_service *io_service_, ConnectionPool_T pool_): io_service(io_service_), pool(pool_)
{
    working_ptr.store(&First);
    UpdateDaughterRangeCache();
    condition_working = true;
    boost::thread *UpdaterThread = new boost::thread(   boost::bind(&DbDataDaughterCache::UpdateDaughterRangeCacheWorker, this) , nullptr );

}

DbDataDaughterCache::~DbDataDaughterCache()
{

}

mccmnc DbDataDaughterCache::GetDaughterRangeCache(std::string phone, mccmnc origin)
{
    DaugterCacheContainer *working_container = working_ptr.load();
    return working_container->get(phone, origin);
}

void DbDataDaughterCache::GetDaughterTables()
{
    DbDaughterTables.clear();


    Connection_T con = nullptr;
    while (con == nullptr) con = ConnectionPool_getConnection(pool);

    TRY
    {
        // looking dafault data
        ResultSet_T r_data = Connection_executeQuery(con, "%s", "SHOW TABLES like '%daughter'");
        while (ResultSet_next(r_data))
        {
            const char *daughter_table =  ResultSet_getString(r_data, 1);
            if (daughter_table != NULL) DbDaughterTables.push_back(  daughter_table );
        }

    }
    CATCH(SQLException)
    {
        LOG(WARNING) << "Failed: SHOW TABLES like '%daughter'" << Exception_frame.message;
    }
    FINALLY
    {
        Connection_close(con);
    }
    END_TRY;


}

void DbDataDaughterCache::GetDaughterTableRecords(std::string tablename)
{

    std::string sql_default = "select  `mcc`, `mnc`, `parent_mcc`, `parent_mnc` from " + tablename;
    Connection_T con = ConnectionPool_getConnection(pool);

    TRY
    {
        // looking dafault data
        ResultSet_T r_data = Connection_executeQuery(con, "%s", sql_default.c_str());
        while (ResultSet_next(r_data))
        {
             daughterDataRecord tmp_data;

             tmp_data.db_mcc = ResultSet_getIntByName(r_data, "mcc");
             tmp_data.db_mnc = ResultSet_getIntByName(r_data, "mnc");
             tmp_data.db_parent_mcc = ResultSet_getIntByName(r_data, "parent_mcc");
             tmp_data.db_parent_mnc = ResultSet_getIntByName(r_data, "parent_mnc");
             daughterDataRecords.push_back(tmp_data);
        }
    }
    CATCH(SQLException)
    {
        LOG(WARNING) << "Failed: " << sql_default <<  Exception_frame.message;
        throw std::runtime_error(Exception_frame.message);
    }
    FINALLY
    {
        Connection_close(con);
    }
    END_TRY;



}



void DbDataDaughterCache::UpdateDaughterRangeCache()
{
    GetDaughterTables();

    if (DbDaughterTables.size() > 0 )  // Db tables ok
    try
    {
        // Clean second buffer
        if ( working_ptr == &First)
            Second.clean();
        else
            First.clean();

        // Fill of data
        for(const auto &table : DbDaughterTables)
        {
                GetDaughterTableRecords(table);
                if ( working_ptr == &First)
                    Second.update(table, daughterDataRecords);
                else
                    First.update(table, daughterDataRecords);
        }

        // Switch to second buffer
        if ( working_ptr == &First)
            working_ptr.store(&Second);
        else
            working_ptr.store(&First);

        LOG(INFO) << "DB cache for daughter update done";
    } catch (...) {
        LOG(WARNING) << "DB cache for daughter update failed leaved old buffer ";

    }


}

void DbDataDaughterCache::UpdateDaughterRangeCacheWorker()
{
    while(condition_working)
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(300000)); // 5 min
        UpdateDaughterRangeCache();
    }
}
