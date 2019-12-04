#include "ProcessingTypeContainer.hpp"



ProcessingTypeContainer::ProcessingTypeContainer()
{

}

std::string ProcessingTypeContainer::getType(std::string phone)
{

    uint64_t int_phone = boost::lexical_cast<uint64_t>(phone);
    LOG(INFO) << "uint64_t int_phone -" << int_phone;
    for (const auto &Record : ProcessingTypeData)
    {
        LOG(INFO) << Record.from << int_phone << Record.to;
        if ( (Record.from >= int_phone ) &&  (Record.to <= int_phone) )
        {

            return Record.type;


        }
    }

    return std::string("DB");
}

void ProcessingTypeContainer::update(std::vector<ProcessingTypeRecord> data)
{
    ProcessingTypeData.clear();
    ProcessingTypeData.swap(data);
}
