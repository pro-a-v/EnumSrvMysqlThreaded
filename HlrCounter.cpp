#include "HlrCounter.hpp"

HlrCounter::HlrCounter()
{

}

unsigned int HlrCounter::hlr_get_num()
{
    return ++id;
}
