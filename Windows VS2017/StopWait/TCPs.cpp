#include "stdafx.h"
#include "Global.h"
#include "TCPs.h"

TCPsenders::TCPsenders()
    :base(0), nextseqnum(0), acksacc(0)
{
    memset(this->packetWaitingAck, 0, sizeof(this->packetWaitingAck));
}

TCPsenders::~TCPsenders()
{

}

bool TCPsenders::getWaitingState()
{
    return !((nextseqnum + SEQUN - base) % SEQUN < WIDEN);
}

bool TCPsenders::send(const Message& message)
{
	if (getWaitingState()) { //发送方处于等待确认状态
		std::cout << "在等待接受中" << endl;
		return false;
	}
	else
	{
		std::cout << "\n[SENDER]发送前窗口：";
		printSlideWindow();
		this->packetWaitingAck[nextseqnum].acknum = -1;                  //忽略该字段
		this->packetWaitingAck[nextseqnum].seqnum = this->nextseqnum;
		this->packetWaitingAck[nextseqnum].checksum = 0;
		memcpy(this->packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
		this->packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextseqnum]);
		pUtils->printPacket("发送方发送报文", this->packetWaitingAck[nextseqnum]);
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
		if (base == nextseqnum)
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->nextseqnum);			//启动发送方定时器
		this->nextseqnum = (this->nextseqnum + 1) % SEQUN;

		std::cout << "[SENDER]发送后窗口：";
		printSlideWindow();
		cout << endl;

		return true;
	}
}

void TCPsenders::receive(const Packet& ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	int sum = ackPkt.acknum;
	if (checkSum != ackPkt.checksum)
	{
		pUtils->printPacket("\n[Debug]数据校验错误-接收的ack损坏", ackPkt);
	}
	else
	{
		if ((sum+SEQUN-this->base)%SEQUN < WIDEN)    //在滑动窗口内，移动滑动窗口
		{
			pns->stopTimer(SENDER, base);
			base = (ackPkt.acknum + 1) % SEQUN;
			
			if (base != this->nextseqnum)
			{
				pns->startTimer(SENDER, Configuration::TIME_OUT, base);
			}
			this->acksacc = 0;
			cout << "\n[SENDER]收到ack:" << ackPkt.acknum << "，移动滑动窗口：";
			printSlideWindow();
			cout << endl;
		}
		else
		{      //接受到冗余的ack
			this->acksacc = (this->acksacc + 1) % 3;
			if (this->acksacc == 2)      //快速重传
			{
				std::cout << "\n[ERROR]收到连续三个冗余ack，快速重传\n";
				pUtils->printPacket("快速重传", this->packetWaitingAck[base]);
				pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[base]);
				
			}
		}
		
	}
}

void TCPsenders::timeoutHandler(int seqNum)
{
	std::cout << "\n[ERROR]出现超时" << endl;
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[base]);
	pUtils->printPacket("[Debug]超时重传的分组", packetWaitingAck[base]);
}

void TCPsenders::printSlideWindow()
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