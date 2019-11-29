#ifndef DAUGHTER_RANGE_CACHE_DATA_HPP
#define DAUGHTER_RANGE_CACHE_DATA_HPP
#include <stdint.h>


struct daughterDataRecord {
    uint16_t db_mcc = 0;
    uint8_t db_mnc = 0;
    uint16_t db_parent_mcc = 0;
    uint8_t db_parent_mnc = 0;
};

#endif // DAUGHTER_RANGE_CACHE_DATA_HPP








