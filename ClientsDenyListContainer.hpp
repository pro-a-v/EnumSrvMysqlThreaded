#ifndef CLIENTSDENYLISTCONTAINER_HPP
#define CLIENTSDENYLISTCONTAINER_HPP

#include <iostream>
#include <string.h>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <map>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

class ClientsDenyListContainer
{
public:
    ClientsDenyListContainer();
    bool isAlowed(std::string ip, std::string phone);
    void update(std::map<std::string, std::string> Data);

private:
        std::map<std::string, std::string> ClientsDenyListData;
};

#endif // CLIENTSDENYLISTCONTAINER_HPP
