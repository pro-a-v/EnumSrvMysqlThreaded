#ifndef ZBXSENDER_HPP
#define ZBXSENDER_HPP

#include <string>
#include <queue>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <atomic>
#include <chrono>

class ZbxSender
{
public:
    ZbxSender();
    std::atomic<bool> ready;
    void setServer(std::string ZabbixHost_, int ZabbixPort_);
    void setHostKey(std::string MonitoringHost_, std::string MonitoringKey_);
    void send(std::string data);


    ~ZbxSender();
private:
    std::queue<std::string> queue;
    std::string TextError;
    std::string ZabbixHost;
    int ZabbixPort;
    std::string MonitoringHost;
    std::string MonitoringKey;

    void run();
    bool tcp_send(std::string value);
};

#endif // ZBXSENDER_HPP
