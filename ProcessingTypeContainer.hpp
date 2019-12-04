#ifndef PROCESSINGTYPECONTAINER_HPP
#define PROCESSINGTYPECONTAINER_HPP

#include <iostream>
#include <string.h>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <g3log/g3log.hpp>

#include "processing_type_cache_data.hpp"

class ProcessingTypeContainer
{
public:
    ProcessingTypeContainer();
    std::string getType(std::string phone);
    void update(std::vector<ProcessingTypeRecord> data);
private:
    std::vector<ProcessingTypeRecord> ProcessingTypeData;


};

#endif // PROCESSINGTYPECONTAINER_HPP
