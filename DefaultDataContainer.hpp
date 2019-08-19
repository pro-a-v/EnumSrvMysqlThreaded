#ifndef DEFAULTDATACONTAINER_HPP
#define DEFAULTDATACONTAINER_HPP
#include "mccmnc.hpp"
#include "default_range_cache_data.hpp"


#include <iostream>
#include <string.h>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <mutex>
#include <map>

class DefaultDataContainer
{
public:
    DefaultDataContainer();
    mccmnc get(std::string phone);
    void update(std::string table, std::vector<defaultDataRecord> data);

private:
    std::map<std::string, std::vector<defaultDataRecord>> defaultDataContainer;

};

#endif // DEFAULTDATACONTAINER_HPP
