#ifndef HLR_COUNTER_HPP
#define HLR_COUNTER_HPP
#include <atomic>

class Hlr_Counter
{
public:
    Hlr_Counter();
    unsigned int hlr_get_num();
private:
    std::atomic<unsigned int> id{0};
};

#endif // HLR_COUNTER_HPP
