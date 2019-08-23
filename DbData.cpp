#include "DbData.hpp"

DbData::DbData(ConnectionPool_T pool_): pool(pool_)
{

}

DbData::~DbData()
{

}

std::string DbData::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}


mccmnc DbData::get(std::string phone_, DbDataDefaultCache *DbDataDefaultCache, DbDataDaughterCache *DbDataDaughterCache)
{
    phone = phone_;
    mcc_data.mcc = 0;
    mcc_data.mnc = 0;

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

    table_template = std::string("l") + std::to_string(phone.length()) + std::string("p") + prefix;
    table_default = table_template + std::string("default");
    table_daughter = table_template + std::string("daughter");
    table_data = table_template  + std::string("data");



    if (isPortable())
    {
        mcc_data = DbDataDaughterCache->GetDaughterRangeCache(phone, mcc_data);
    }
    else
    {
        mcc_data = DbDataDefaultCache->GetDefaultRangeCache(phone);
        mcc_data = DbDataDaughterCache->GetDaughterRangeCache(phone, mcc_data);
    }


    // ======================


   if (mcc_data.mcc != 0)
   {
       return mcc_data;
   }

   std::cout <<  "Failed: No data Found for " << phone << std::endl;
   throw std::runtime_error("No data Found for ");


}

bool DbData::isPortable()
{
    // ======================
    // looking portable data
    // ======================
            Connection_T con = ConnectionPool_getConnection(pool);
    TRY

        // looking portable data
        std::string sql_data = "select mcc, mnc from enum."+ table_data + std::string(" where phone=") + phone;

        ResultSet_T r_data = Connection_executeQuery(con, sql_data.c_str());

        if (ResultSet_next(r_data))
        {
             mcc_data.mcc = ResultSet_getIntByName(r_data, "mcc");
             mcc_data.mnc = ResultSet_getIntByName(r_data, "mnc");
             return true;
        }

    CATCH(SQLException)
        std::cout << "Phone " << phone << " error isPortable with " <<  Exception_frame.message;
    END_TRY;
    Connection_close(con);
    return false;



}



