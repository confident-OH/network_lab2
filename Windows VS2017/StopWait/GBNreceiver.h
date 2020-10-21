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
    void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����

public:
    GBNreceiver();
    virtual ~GBNreceiver();
};