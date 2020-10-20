#include "stdafx.h"
#include "Global.h"
#include "GBNsender.h"

GBNsenders::GBNsenders()
{
    this->base = 0;
    this->nextseqnum = 0;
	this->waitingState = false;
	memset(this->packetWaitingAck, 0, sizeof(this->packetWaitingAck));
}

GBNsenders::~GBNsenders()
{
}

bool GBNsenders::getWaitingState() {
	this->waitingState = (nextseqnum + SEQUN - base) % SEQUN < WIDEN;
	return !this->waitingState;
}

bool GBNsenders::send(const Message& message) {
	if (getWaitingState()) { //发送方处于等待确认状态
		std::cout << "在等待接受中" << endl;
		return false;
	}
	else
	{
		//在窗口中设置ACKS
		std::cout << "\n[SENDER]发送前窗口：";
		printSlideWindow();
		this->waitingState = true;
		this->packetWaitingAck[nextseqnum].acknum = -1; //忽略该字段
		this->packetWaitingAck[nextseqnum].seqnum = this->nextseqnum;
		this->packetWaitingAck[nextseqnum].checksum = 0;
		memcpy(this->packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
		this->packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextseqnum]);
		pUtils->printPacket("发送方发送报文", this->packetWaitingAck[nextseqnum]);
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
		if(base == nextseqnum)
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[nextseqnum].seqnum);			//启动发送方定时器
		this->nextseqnum = (this->nextseqnum + 1) % SEQUN;
		std::cout << "[SENDER]发送后窗口：";
		printSlideWindow();
		cout << endl;
		return true;
    }
}

void GBNsenders::receive(const Packet& ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum != ackPkt.checksum)
	{
		pUtils->printPacket("\n[Debug]数据校验错误-接收的ack损坏", ackPkt);
	}
	else
	{
		base = (ackPkt.acknum + 1) % SEQUN;//累积确认，别忘了取模
		if (this->base == this->nextseqnum)
		{
			pns->stopTimer(SENDER, 0);
		}
		else
		{//重启计时器
			pns->stopTimer(SENDER, 0);
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		}
		std::cout << "\n[SENDER]收到ack:" << ackPkt.acknum << "，移动滑动窗口：";
		printSlideWindow();
		std::cout << endl;
	}
}

void GBNsenders::timeoutHandler(int seqNum)
{
	int i;
	std::cout << "\n[ERROR]出现超时" << endl;
	if (this->nextseqnum == this->base)
	{
		//超时特例，不做处理
		return;
	}
	else {
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
		for (i = this->base; i != this->nextseqnum; i = (i + 1) % SEQUN) {
			pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[i]);
			pUtils->printPacket("[Debug]超时重传的分组:", packetWaitingAck[i]);
		}
	}
	
}

void GBNsenders::printSlideWindow()
{
	int i;
	for (i = 0; i < SEQUN; i++)
	{
		if (i == this->base)
			std::cout << "[";
		if (i == this->nextseqnum)
			std::cout << "|";
		std::cout << i;
		if (i == (base + WIDEN - 1) % SEQUN)
			std::cout << "]";
		std::cout << " ";
	}
	std::cout << std::endl;
}