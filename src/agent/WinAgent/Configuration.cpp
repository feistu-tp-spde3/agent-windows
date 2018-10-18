#include "Configuration.hpp"
#include "pugixml.hpp"
#include "Collector.hpp"

#include <direct.h>
#include <Windows.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

std::mutex messageControlMutex;
std::string clientMessage{ "" };

Configuration::Configuration(const std::string& configurationFilename)
    : mQueueLength(8192)
    , mQueueTime(2048)
    , mSendingTime(60)
    , mDirectory("Data/")
    , mconfigurationFilename(configurationFilename)
{
	std::cout << "[Configuration] Creating configuration" << std::endl;
	parse();
	createFilter();
	createDirectory();
}

void Configuration::parse()
{
    boost::filesystem::path path(boost::filesystem::current_path());
    
    std::string fullPath = path.string() + "/" + mconfigurationFilename;
	pugi::xml_document configurationFile;
	pugi::xml_parse_result result = configurationFile.load_file(fullPath.c_str());

	if (result.status == pugi::xml_parse_status::status_ok)
	{
		pugi::xml_node configuration = configurationFile.child("Configuration");
		pugi::xml_node agent;
		
		if(configuration)
			agent = configuration.child("Agent");

		// nacitanie nastaveni agenta
		if (agent)
		{
			if (agent.child("Name"))
				mAgentName = agent.child("Name").text().as_string();
			if (agent.child("IP_Protocol"))
				mIPProtocol = agent.child("IP_Protocol").text().as_string();
			if (agent.child("SrcAddr"))
				mSrcAddr = agent.child("SrcAddr").text().as_string();
			if (agent.child("DstAddr"))
				mDstAddr = agent.child("DstAddr").text().as_string();
			if (agent.child("CoreProtocol"))
				mCoreProtocol = agent.child("CoreProtocol").text().as_string();
			if (agent.child("SrcPort"))
				mSrcPort = agent.child("SrcPort").text().as_string();
			if (agent.child("DstPort"))
				mDstPort = agent.child("DstPort").text().as_string();
			if (agent.child("Bound"))
				mBound = agent.child("Bound").text().as_string();
			if (agent.child("QueueLength"))
				mQueueLength = agent.child("QueueLength").text().as_uint();
			if (agent.child("QueueTime"))
				mQueueTime = agent.child("QueueTime").text().as_uint();
			if (agent.child("Directory"))
				mDirectory = agent.child("Directory").text().as_string();
		}

		// nacitanie kolektorov
		pugi::xml_node sender = configuration.child("Sender");

		if (sender)
		{
			for (pugi::xml_node collector = sender.child("Collector"); collector; collector = collector.next_sibling("Collector"))
			{
				std::string collectorAddr = collector.text().as_string();
				std::string addr = collectorAddr.substr(0, collectorAddr.find(":", 0));
				std::string port = collectorAddr.substr(collectorAddr.find(":", 0) + 1, collectorAddr.length() - collectorAddr.find(":", 0));
			
				Collector newCollector(addr, port);
				collectors.push_back(newCollector);
			}

            if (sender.child("SendingTime"))
            {
                mSendingTime = atoi(sender.child("SendingTime").text().as_string());
            }
		}
	}
	else 
    {
		std::cout << "[Configuration] Could not parse configuration file: " << result.description() << std::endl;
	}
}

void Configuration::createFilter()
{
    if (!mBound.empty())
    {
        mFilter += mBound;
    }
		
    if (!mIPProtocol.empty())
    {
        if (mIPProtocol == "IPv4")
        {
            mFilter += " and ip";
        }

        if (mIPProtocol == "IPv6")
        {
            mFilter += " and ipv6";
        }
	}

    if (!mSrcAddr.empty())
    {
        mFilter += " and ip.SrcAddr == " + mSrcAddr;
    }
		

    if (!mDstAddr.empty())
    {
        mFilter += " and ip.DstAddr == " + mDstAddr;
    }
		

	if (!mCoreProtocol.empty())
	{
		if (mCoreProtocol == "TCP")
		{
            if (!mSrcPort.empty())
            {
                mFilter += "and tcp.SrcPort == " + mSrcPort;
            }
				
            if (!mDstPort.empty())
            {
                mFilter += "and tcp.DstPort == " + mDstPort;
            }
		}
		else if (mCoreProtocol == "UDP")
		{
            if (!mSrcPort.empty())
            {
                mFilter += "and udp.SrcPort == " + mSrcPort;
            }
				
            if (!mDstPort.empty())
            {
                mFilter += "and udp.DstPort == " + mDstPort;
            }	
		} 
		else if (mCoreProtocol == "ICMP")
		{
			mFilter += " and icmp";
		}
	}

	// v pripade ak sme nemali nastaveny konfiguracny subor filtrujeme celu prevadzku
	if (mFilter.empty())
	{
		std::string ip = getLocalIP();
		mFilter = "ip.SrcAddr == " + ip + " or ip.DstAddr == " + ip;
	}

	std::cout << "[Configuration] Filter created from configuration file: " << mFilter << std::endl;
}

std::string Configuration::getLocalIP() const
{
	WSADATA wsaData;
    const WORD requestedVersion = 0x101;
    const int hostnameLenght = 255;
    char hostName[hostnameLenght];

	WSAStartup(requestedVersion, &wsaData);
	gethostname(hostName, hostnameLenght);

	struct hostent *host_entry;
	host_entry = gethostbyname(hostName);
	char *ip = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);
	WSACleanup();

	return std::string(ip);
}

void Configuration::createDirectory()
{
    boost::filesystem::path path(boost::filesystem::current_path());
	std::string dataPath = path.string() + "\\" + mDirectory;

	int ret = _mkdir(dataPath.c_str());
	
    if (ret == EEXIST)
    {
        std::cout << "[Configuration] Directory " << dataPath << " already exists!" << std::endl;
    }
	else if (ret == ENOENT)
	{
		std::cout << "[Configuration] Path " << dataPath << " was not found!" << std::endl;
		mDirectory = "";
	}
}