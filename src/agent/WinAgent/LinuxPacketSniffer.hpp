#pragma once
//TODO

#include "PacketSniffer.hpp"

class LinuxPacketSniffer : public PacketSniffer
{
public:
    bool init(const Configuration &config) override;
    void run(const Configuration &config) override;
};