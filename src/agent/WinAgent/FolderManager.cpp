#include "ConfigManager.hpp"

#include <boost/filesystem.hpp>
#include <chrono>

ConfigManager::ConfigManager(unsigned int cycleTimeSecond)
    : mSeconds(cycleTimeSecond)
{

}

bool ConfigManager::IsChangedConfig()
{
    //TODO: path not hardcoded
    std::string path = "D:\\agent-windows\\src\\agent\\WinAgent\\config_agent.xml";
    time_t timeWrittenSeconds = boost::filesystem::last_write_time(path);
    auto timeSinceEpochSeocnds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    if ((timeSinceEpochSeocnds - timeWrittenSeconds) < mSeconds)
    {
        return true;
    }

    return false;
}

void ConfigManager::Start()
{
    mThread = boost::thread([this]
    {
        boost::this_thread::sleep_for(boost::chrono::seconds(mSeconds));
        if (IsChangedConfig())
        {
            //Reload configuration
        }
    });
}

void ConfigManager::Stop()
{
    mThread.interrupt();
    mThread.join();
}