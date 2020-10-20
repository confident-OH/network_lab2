#pragma once
#define SEQUN 0x8
#define WIDEN 4
#include "RdtSender.h"

class SRsenders
    :public RdtSender
{
private:
    Packet packetWaitingAck[SEQUN];	//�ѷ��Ͳ��ȴ�Ack�����ݰ�
    int base;                       //��������
    int nextseqnum;                 //��һ��������
    bool label[SEQUN];

private:
    void printSlideWindow();

public:
    bool send(const Message& message);	//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
    void receive(const Packet& ackPkt); //����ȷ��Ack������NetworkServiceSimulator����	
    void timeoutHandler(int seqNum);	//Timeout handler������NetworkServiceSimulator����
    bool getWaitingState();
    SRsenders();
    virtual ~SRsenders();
};