#ifndef DEFAULT_RANGE_CACHE_DATA_HPP
#define DEFAULT_RANGE_CACHE_DATA_HPP
#include <stdint.h>


struct defaultDataRecord {
    uint16_t db_mcc = 0;
    uint8_t db_mnc = 0;
    uint64_t db_from = 0;
    uint64_t db_to = 0;
};

#endif // DEFAULT_RANGE_CACHE_DATA_HPP








