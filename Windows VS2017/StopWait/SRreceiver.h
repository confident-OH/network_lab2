#pragma once
#define SEQUN 0x8
#define WIDEN 4 
#include "RdtReceiver.h"
extern FILE* fp;

class SRreceiver
    :public RdtReceiver
{
private:
    int expectebase;        //base��
    Packet lastAckPkt;      //��һ��
    Message massn[SEQUN];   //message������
    bool isrvc[SEQUN];      //��ʾ�������е�i��message�Ƿ���ܵ�

public:
    void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����

public:
    SRreceiver();
    virtual ~SRreceiver();

};