#ifndef DBDATA_PROCESSINGTYPE_CACHE_HPP
#define DBDATA_PROCESSINGTYPE_CACHE_HPP
#include <boost/asio.hpp>

#include <zdb/zdb.h>
#include <zdb/SQLException.h>
#include <g3log/g3log.hpp>
#include <boost/thread.hpp>

#include "ProcessingTypeContainer.hpp"
#include "processing_type_cache_data.hpp"

class DbData_ProcessingType_Cache
{
public:
    DbData_ProcessingType_Cache(boost::asio::io_service *io_service, ConnectionPool_T pool_);
    std::string Get_ProcessingType(std::string phone);
private:
    // ----------------------
    boost::asio::io_service *io_service;
    ConnectionPool_T pool;


    ProcessingTypeContainer First, Second;
    std::atomic<ProcessingTypeContainer*> working_ptr;
    std::vector<ProcessingTypeRecord> ProcessingType_DbData;

    bool condition_working;


    void Get_DB_ProcessingType_List();
    void Update_DB_ProcessingType();
    void Update_DB_ProcessingType_Worker();

};

#endif // DBDATA_PROCESSINGTYPE_CACHE_HPP
