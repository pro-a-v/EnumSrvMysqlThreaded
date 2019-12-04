#include "DbData_ProcessingType_Cache.hpp"



DbData_ProcessingType_Cache::DbData_ProcessingType_Cache(boost::asio::io_service *io_service_, ConnectionPool_T pool_): io_service(io_service_), pool(pool_)
{

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
        ResultSet_T r_data = Connection_executeQuery(con, "%s", "SELECT id, `from`, `to`, `type` FROMM enum.requestProcessingType;");
        const char *process_type;

        while (ResultSet_next(r_data))
        {
            ProcessingTypeRecord tmp;
            tmp.type = std::string(ResultSet_getStringByName(r_data, "type"));
            tmp.from = ResultSet_getLLongByName(r_data, "from");
            tmp.to = ResultSet_getLLongByName(r_data, "to");


            if ( process_type != NULL)
                    ClientsDenyListData.insert(std::make_pair( ip_data , CountryPrefixDeny_data));
            else
                LOG(WARNING) << "Failed: select `IP`, `CountryPrefixDeny` from clientsDenyList";
        }
    }
    CATCH(SQLException)
    {
        LOG(WARNING) << "Failed: select `IP`, `CountryPrefixDeny` from clientsDenyList" <<  Exception_frame.message;
        //throw std::runtime_error(sql_default);
    }
    FINALLY
    {
        Connection_close(con);
    }
    END_TRY;
}
