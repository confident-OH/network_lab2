#pragma once
#define SEQUN 8
#define WIDEN 4
#include "RdtSender.h"
extern FILE* fp;

class GBNsenders 
	:public RdtSender
{
private:
	Packet packetWaitingAck[SEQUN];	//已发送并等待Ack的数据包
	int base;                       //窗口奇数
	int nextseqnum;                 //下一个序列数
	bool waitingState;

private:
	void printSlideWindow();
public:
	bool send(const Message& message);	//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet& ackPkt); //接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);	//Timeout handler，将被NetworkServiceSimulator调用
	bool getWaitingState();
public:
	GBNsenders();
	virtual ~GBNsenders();
};