#pragma once

#include <string>
#include <iostream>
#include <mutex>

// mutex na riadenie sprav od klienta
extern std::mutex messageControlMutex;

// sprava od klienta
extern std::string clientMessage;

class Configuration 
{
private:
	std::string mAgentName;		    // nazov agenta
	std::string mIPProtocol;		// filtrovanie podla IP protokolu(IPv4, IPv6)
	std::string mSrcAddr;			// filtrovanie podla zdrojovej adresy
	std::string mDstAddr;			// filtrovanie podla cielovej adresy
	std::string mCoreProtocol;	    // filtrovanie podla vnoreneho protokolu(TCP,UDP,...)
	std::string mSrcPort;			// filtrovanie podla zdrojoveho portu
	std::string mDstPort;			// filtrovanie podla cieloveho portu
	std::string mBound;			    // filtrovanie prichadzajucich/odchadzajucich paketov
    const std::string mconfigurationFilename;

	unsigned mQueueLength;		    // velkost fronty na pakety
	unsigned mQueueTime;		    // maximalny cas paketu vo fronte
	unsigned mSendingTime;			// cas po ktorom sa odosielaju data na kolektor

	std::string mDirectory;		    // adresar kam sa ukladaju pakety

	// filter pre windivert
	std::string mFilter;

	// parsovanie xml dokumentu
	void parse();

	// vytvorenie filtra
	void createFilter();

	// vytvorenie adresara na ukladanie paketov
	void createDirectory();

	// ziskanie lokalnej ipcky
	std::string getLocalIP() const;

public:
	Configuration(const std::string& configurationFilename);

	std::string getFilter() const { return mFilter; }

	unsigned getQueueLenght() const { return mQueueLength; }
	unsigned getQueueTime() const { return mQueueTime; }
	unsigned getSendingTime() const { return mSendingTime; }

	std::string getDirectory() const { return mDirectory; }
	std::string getAgentName() const { return mAgentName; }
};
