#include "DaugterCacheContainer.hpp"

DaugterCacheContainer::DaugterCacheContainer()
{

}



mccmnc DaugterCacheContainer::get(std::string phone, mccmnc origin)
{
    std::string prefix;
    mccmnc tmp = origin;
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

    std::string table_name = std::string("l") + std::to_string(phone.length()) + std::string("p") + prefix + std::string("daughter");

    if ( daughterData.find(table_name) != daughterData.end() )
    try {
        std::vector<daughterDataRecord> daughterDataRecords = daughterData[table_name];


        for (const auto &Record : daughterDataRecords)
        {
            if (Record.db_mnc == origin.mnc )
            if (Record.db_mcc == origin.mcc )
            {
                tmp.mcc = Record.db_parent_mcc;
                tmp.mnc = Record.db_parent_mnc;

            }
        }

    } catch (...) {}



    return tmp;
}

void DaugterCacheContainer::update(std::string table, std::vector<daughterDataRecord> data)
{
        daughterData[table].clear();
        daughterData[table].swap(data);
}

void DaugterCacheContainer::clean()
{
    daughterData.clear();
}
