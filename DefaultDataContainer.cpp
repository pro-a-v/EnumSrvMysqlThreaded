#include "DefaultDataContainer.hpp"

DefaultDataContainer::DefaultDataContainer()
{

}

mccmnc DefaultDataContainer::get(std::string phone)
{
    std::string prefix;
    mccmnc tmp;
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

    std::string table_name = std::string("l") + std::to_string(phone.length()) + std::string("p") + prefix + std::string("default");

    if ( defaultDataContainer.find(table_name) != defaultDataContainer.end() )
    try {
        std::vector<defaultDataRecord> tableDataRecords = defaultDataContainer[table_name];

        uint64_t int_phone = std::stoull(phone);

        for (const auto &Record : tableDataRecords)
        {

            if (Record.db_from <= int_phone )
            if (Record.db_to >= int_phone )
            {
                tmp.mcc = Record.db_mcc;
                tmp.mnc = Record.db_mnc;

            }
        }

    }
    catch(const std::exception& ex)
    {
        LOG(WARNING) << "DefaultDataContainer::get: Error occurred: " << ex.what() << std::endl;
    }



    return tmp;
}

void DefaultDataContainer::update(std::string table, std::vector<defaultDataRecord> data)
{
    defaultDataContainer[table].clear();
    defaultDataContainer[table].swap(data);
}

void DefaultDataContainer::clean()
{
    defaultDataContainer.clear();
}
