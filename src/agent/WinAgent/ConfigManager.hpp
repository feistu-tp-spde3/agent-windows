#pragma once

#include <boost/thread.hpp>

class ConfigManager
{
private:
    boost::thread mThread;
    unsigned int mSeconds;

    bool IsChangedConfig();
public:
    ConfigManager(unsigned int cycleTimeSecond = 60 * 60);
    void Start();
    void Stop();
};