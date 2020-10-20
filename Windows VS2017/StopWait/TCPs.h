#pragma once
#include "RdtSender.h"
#define SEQUN 0x8
#define WIDEN 4

class TCPsenders
    :public RdtSender
{
private: 
    int base;
    int nextseqnum;
    Packet packetWaitingAck[SEQUN];
    int acksacc;
private:
    void printSlideWindow();
public:
    bool send(const Message &message);
    bool getWaitingState();
    void receive(const Packet& ackPkt);
    void timeoutHandler(int seqNum);
    TCPsenders();
    virtual ~TCPsenders();
};