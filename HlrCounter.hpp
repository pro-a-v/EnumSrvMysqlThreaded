#ifndef HLRCOUNTER_HPP
#define HLRCOUNTER_HPP
#include <atomic>

class HlrCounter
{
public:
    HlrCounter();
    unsigned int hlr_get_num();
private:
    std::atomic<unsigned int> id{0};
};

#endif // HLRCOUNTER_HPP
