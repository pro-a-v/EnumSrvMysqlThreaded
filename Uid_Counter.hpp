#ifndef UID_COUNTER_HPP
#define UID_COUNTER_HPP
#include <atomic>

class Uid_Counter
{
public:
    Uid_Counter();
    unsigned int get_uid();
private:
    std::atomic<unsigned int> id{0};
};

#endif // UID_COUNTER_HPP
