#pragma once
#include "RdtReceiver.h"
#define SEQUN 8
#define WIDEN 4

extern FILE* fp;

class GBNreceiver :public RdtReceiver {
private:
    int expectedseq;
    Packet lastAckPkt;
public:
    void receive(const Packet& packet);	//接收报文，将被NetworkService调用

public:
    GBNreceiver();
    virtual ~GBNreceiver();
};