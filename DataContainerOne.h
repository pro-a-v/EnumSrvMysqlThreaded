#ifndef DATACONTAINERONE_H
#define DATACONTAINERONE_H

#include <fstream>
#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <mutex>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/date_time.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/regex.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/foreach.hpp>
#include <cmath>
#include <g3log/g3log.hpp>


static void trim(std::string &s) {
    if (!s.empty())
    {
     s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), [](char c){ return std::isspace(c); }));
     s.erase(std::find_if_not(s.rbegin(), s.rend(), [](char c){ return std::isspace(c); }).base(), s.end());
    }
}

class DataContainerOne
{


private:
    boost::posix_time::ptime portable_data;


    struct default_data { uint64_t from; uint64_t to; uint8_t mcc; } ;
    std::vector <default_data> portable_array;
    std::vector <default_data> portable_array_;

    std::map <uint64_t ,uint8_t>  portable_svyazkom_mnc_;
    std::map <uint64_t ,uint8_t> portable_svyazkom_mnc;
    // portable_array.insert(make_pair("sin",ptr_fun(sin)));


    boost::asio::signal_set *_signals_reload;
    std::mutex PortableMutex;


    void LoadFile();
    void LoadDefaultFileWorker();
    void LoadPortableWorker();
    void string_toupper(std::string &s, const std::locale &loc);

    void LoadPortableFile(std::string filename);
    uint8_t XmlPortableFilePrefixes(std::string xml_operator);


    bool is_digits(const std::string &str);

    std::vector<std::string>&get_file_list(const std::string& path, std::vector<std::string>&m_file_list);

public:
    DataContainerOne(boost::asio::signal_set *signals_reload);
    uint8_t GetImsi(uint64_t number);
};

#endif // DATACONTAINERONE_H
