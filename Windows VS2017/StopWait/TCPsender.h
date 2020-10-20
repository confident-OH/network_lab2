#pragma once
#include "RdtSender.h"
#define SEQUN 0x8
#define WIDEN 4

class TCPsender
    :public RdtSender
{
private: 
    int base;
    int nextseqnum;
    Packet packetWaitingAck[SEQUN];
private:
    void printSlideWindow();
public:
    bool send(Message &message);
    bool getWaitingState();
    void receive(Packet& ackPkt);
    void timeoutHandler();
    TCPsender();
    virtual ~TCPsender();
};