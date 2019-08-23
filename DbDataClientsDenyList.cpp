#include "DbDataClientsDenyList.hpp"

DbDataClientsDenyList::DbDataClientsDenyList(boost::asio::io_service *io_service_, ConnectionPool_T pool_): pool(pool_), io_service(io_service_)
{
    UpdateClientsDenyList();
    condition_working = true;
    boost::thread *UpdaterThread = new boost::thread(   boost::bind(&DbDataClientsDenyList::UpdateClientsDenyListWorker, this) , nullptr );

}

DbDataClientsDenyList::~DbDataClientsDenyList()
{
    ;
}

bool DbDataClientsDenyList::isAlowed(std::string ip, std::string phone)
{
    ClientsDenyListContainer *working_container = working_ptr;
    return working_container->isAlowed(ip, phone);
}

void DbDataClientsDenyList::GetDBClientsDenyList()
{
    ClientsDenyListData.clear();

    std::string sql_default;

    Connection_T con = nullptr;
    while (con == nullptr) con = ConnectionPool_getConnection(pool);


    TRY
    {
        // looking dafault data
        sql_default = "select `IP`, `CountryPrefixDeny` from clientsDenyList";
        ResultSet_T r_data = Connection_executeQuery(con, "%s", sql_default.c_str());
        while (ResultSet_next(r_data))
        {
             ClientsDenyListData.insert(std::make_pair( ResultSet_getStringByName(r_data, "IP") , ResultSet_getStringByName(r_data, "CountryPrefixDeny")));
        }
    }
    CATCH(SQLException)
    {
        std::cout << "Failed: " << sql_default <<  Exception_frame.message;
        //throw std::runtime_error(sql_default);
    }
    FINALLY
    {
        Connection_close(con);
    }
    END_TRY;


}

void DbDataClientsDenyList::UpdateClientsDenyList()
{
    GetDBClientsDenyList();
    if (ClientsDenyListData.size() > 0) // No Errors when get DB Data
    {
        if ( working_ptr == &First)
        {
            Second.update(ClientsDenyListData);
            working_ptr.store(&Second);
        }
        else
        {
            First.update(ClientsDenyListData);
            working_ptr.store(&First);
        }
    }
}


void DbDataClientsDenyList::UpdateClientsDenyListWorker()
{
    while(condition_working)
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(60000)); // 1 min
        UpdateClientsDenyList();
        std::cout << "DB cache for ClientsDenyList update done" << std::endl;
    }
}
