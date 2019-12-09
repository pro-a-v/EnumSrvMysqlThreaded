#include "Uid_Counter.hpp"

Uid_Counter::Uid_Counter()
{

}

unsigned int Uid_Counter::get_uid()
{
    return ++id;
}
