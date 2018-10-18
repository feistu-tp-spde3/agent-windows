#pragma once
//TODO

#include "PacketSniffer.h"

class LinuxPacketSniffer : public PacketSniffer
{
public:
    bool init(const Configuration &config) override;
    void run(const Configuration &config) override;
};