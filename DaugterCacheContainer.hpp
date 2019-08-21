#ifndef DAUGTERCACHECONTAINER_HPP
#define DAUGTERCACHECONTAINER_HPP
#include "mccmnc.hpp"
#include "daughter_range_cache_data.hpp"
#include <iostream>
#include <string.h>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <mutex>
#include <map>


class DaugterCacheContainer
{
public:
    DaugterCacheContainer();
    mccmnc get(std::string phone, mccmnc origin);
    void update(std::string table, std::vector<daughterDataRecord> data);
    void clean();

private:
    std::vector<daughterDataRecord> daughterDataRecords;
    std::map<std::string, std::vector<daughterDataRecord> > daughterData;

};

#endif // DAUGTERCACHECONTAINER_HPP
