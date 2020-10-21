#pragma once
#define SEQUN 0x8
#define WIDEN 4 
#include "RdtReceiver.h"
extern FILE* fp;

class SRreceiver
    :public RdtReceiver
{
private:
    int expectebase;        //base点
    Packet lastAckPkt;      //上一个
    Message massn[SEQUN];   //message缓冲区
    bool isrvc[SEQUN];      //表示缓冲区中第i号message是否接受到

public:
    void receive(const Packet& packet);	//接收报文，将被NetworkService调用

public:
    SRreceiver();
    virtual ~SRreceiver();

};