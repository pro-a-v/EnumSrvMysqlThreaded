#include "DbData_ProcessingType_Cache.hpp"



DbData_ProcessingType_Cache::DbData_ProcessingType_Cache(boost::asio::io_service *io_service_, ConnectionPool_T pool_): io_service(io_service_), pool(pool_)
{

    working_ptr.store(&First);
    Update_DB_ProcessingType();
    condition_working = true;
    boost::thread *UpdaterThread = new boost::thread(   boost::bind(&DbData_ProcessingType_Cache::Update_DB_ProcessingType_Worker, this) , nullptr );


}

void DbData_ProcessingType_Cache::Get_DB_ProcessingType_List()
{
    ProcessingType_DbData.clear();

    //std::string sql_default;

    Connection_T con = nullptr;
    while (con == nullptr) con = ConnectionPool_getConnection(pool);


    TRY
    {
        // looking dafault data
        ResultSet_T r_data = Connection_executeQuery(con, "%s", "SELECT id, `from`, `to`, `type` FROM requestProcessingType");
        const char *process_type;

        while (ResultSet_next(r_data))
        {
            ProcessingTypeRecord tmp;
            const char *str_check = nullptr;
            str_check = ResultSet_getStringByName(r_data, "type");
            tmp.from = ResultSet_getLLongByName(r_data, "from");
            tmp.to = ResultSet_getLLongByName(r_data, "to");


            if (  str_check != NULL)
            {
                tmp.type = std::string(str_check);
                ProcessingType_DbData.push_back(tmp);
            }
            else
                LOG(WARNING) << "Failed: SELECT id, `from`, `to`, `type` FROM requestProcessingType";
        }
    }
    CATCH(SQLException)
    {
        LOG(WARNING) << "Failed: SELECT id, `from`, `to`, `type` FROM requestProcessingType" <<  Exception_frame.message;
        //throw std::runtime_error(sql_default);
    }
    FINALLY
    {
        Connection_close(con);
    }
    END_TRY;
}

void DbData_ProcessingType_Cache::Update_DB_ProcessingType()
{
    try{
        Get_DB_ProcessingType_List();
        if (ProcessingType_DbData.size() > 0) // No Errors when get DB Data
        {
            if ( working_ptr == &First)
            {
                Second.update(ProcessingType_DbData);
                working_ptr.store(&Second);
            }
            else
            {
                First.update(ProcessingType_DbData);
                working_ptr.store(&First);
            }
        }
        LOG(INFO) << "DB cache for ProcessingType update done";
    }
    catch(const std::exception& ex)
    {
        LOG(WARNING) << "DbData_ProcessingType_Cache::Update_DB_ProcessingType Error occurred: " << ex.what();
    }
}

void DbData_ProcessingType_Cache::Update_DB_ProcessingType_Worker()
{
    while(condition_working)
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(60000)); // 1 min
        Update_DB_ProcessingType();
    }
}
