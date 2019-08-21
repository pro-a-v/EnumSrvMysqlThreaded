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
    ;
}

mccmnc DbDataDaughterCache::GetDaughterRangeCache(std::string phone, mccmnc origin)
{
    DaugterCacheContainer *working_container = working_ptr;
    return working_container->get(phone, origin);
}

void DbDataDaughterCache::GetDaughterTables()
{
    DbDaughterTables.clear();
    std::string sql_default = "SHOW TABLES like '%daughter'";
    Connection_T con = ConnectionPool_getConnection(pool);
    TRY
        // looking dafault data
        ResultSet_T r_data = Connection_executeQuery(con, "%s", sql_default.c_str());
        while (ResultSet_next(r_data))
        {
             DbDaughterTables.push_back(  ResultSet_getString(r_data, 1) );
        }

    CATCH(SQLException)
        std::cout << "Failed: " << sql_default <<  Exception_frame.message;
        // throw std::runtime_error(Exception_frame.message);
    END_TRY;
    Connection_close(con);

}

void DbDataDaughterCache::GetDaughterTableRecords(std::string tablename)
{

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
             daughterDataRecords.push_back(tmp_data);
        }

    CATCH(SQLException)
        std::cout << "Failed: " << Exception_frame.message;
        throw std::runtime_error(Exception_frame.message);
    END_TRY;
    Connection_close(con);


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

         std::cout << "DB cache for daughter update done" << std::endl;
    } catch (...) {
        std::cout << "DB cache for daughter update failed leaved old buffer " << std::endl;

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
