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
    std::lock_guard<std::mutex> guard(ClientsDenyListDataMutex);
    if (ClientsDenyListDataContainer.find( ip ) != ClientsDenyListDataContainer.end())
    {
        // We have such ip - check for deny range
        std::string ranges = ClientsDenyListDataContainer[ip];

        if ( ranges == std::string("") ) // ALL allowed
                return true;

        std::vector <std::string> raw_string_parts;
        //split by coma
        split(raw_string_parts, ranges, boost::is_any_of(","));
        // trim for spaces
        std::for_each(raw_string_parts.begin(), raw_string_parts.end(), boost::bind(&boost::trim<std::string>, _1, std::locale() ));

        // Check each range
        for(std::vector<std::string>::iterator it = raw_string_parts.begin(); it != raw_string_parts.end(); ++it) {
            std::string range = (*it);
            if (range == phone.substr(0, range.length()) )
                return false;
        }

    }
    else {
        std::cout << "Couldn't find " << ip << " in clientsDenyList table";
        return false;
    }


    return true;
}



void DbDataClientsDenyList::UpdateClientsDenyList()
{

    std::string sql_default;
    Connection_T con = ConnectionPool_getConnection(pool);
    TRY
        // looking dafault data
        sql_default = "select `IP`, `CountryPrefixDeny` from clientsDenyList";

        ResultSet_T r_data = Connection_executeQuery(con, "%s", sql_default.c_str());

        std::lock_guard<std::mutex> guard(ClientsDenyListDataMutex);

        int rows = Connection_getFetchSize(con);
        if ( rows > 0)     ClientsDenyListDataContainer.clear();


        while (ResultSet_next(r_data))
        {
             ClientsDenyListDataContainer.insert(std::make_pair( ResultSet_getStringByName(r_data, "IP") , ResultSet_getStringByName(r_data, "CountryPrefixDeny")));
        }

    CATCH(SQLException)
        std::cout << "Failed: " << sql_default <<  Exception_frame.message;
        //throw std::runtime_error(sql_default);
    END_TRY;
    Connection_close(con);


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
