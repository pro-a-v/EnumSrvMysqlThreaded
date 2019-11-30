#ifndef PROCESSING_TYPE_CACHE_DATA_HPP
#define PROCESSING_TYPE_CACHE_DATA_HPP
#include <string>
#include <inttypes.h>

struct ProcessingTypeRecord {
    uint64_t from = 0;
    uint64_t to = 0;
    std::string type;

};
#endif // PROCESSING_TYPE_CACHE_DATA_HPP
