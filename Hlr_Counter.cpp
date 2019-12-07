#include "Hlr_Counter.hpp"

Hlr_Counter::Hlr_Counter()
{

}

unsigned int Hlr_Counter::hlr_get_num()
{
    return ++id;
}
