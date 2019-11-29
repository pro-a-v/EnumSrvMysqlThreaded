#include "ClientsDenyListContainer.hpp"

ClientsDenyListContainer::ClientsDenyListContainer()
{

}

bool ClientsDenyListContainer::isAlowed(std::string ip, std::string phone)
{

    if (ClientsDenyListData.find( ip ) != ClientsDenyListData.end())
    {
        // We have such ip - check for deny range
        std::string ranges = ClientsDenyListData[ip];

        if ( ranges == std::string("") ) // ALL allowed
                return true;

        std::vector <std::string> raw_string_parts;
        split(raw_string_parts, ranges, boost::is_any_of(","));  //split by coma
        std::for_each(raw_string_parts.begin(), raw_string_parts.end(), boost::bind(&boost::trim<std::string>, _1, std::locale() )); // trim for spaces

        // Check each range
        for(const auto &range : raw_string_parts)
        {
            if (range == phone.substr(0, range.length()) )
                return false;
        }

    }
    else {
        LOG(INFO) << "Couldn't find " << ip << " in clientsDenyList table";
        return false;
    }


    return true;
}

void ClientsDenyListContainer::update(std::map<std::string, std::string> Data)
{
    ClientsDenyListData.clear();
    ClientsDenyListData.swap(Data);
}
