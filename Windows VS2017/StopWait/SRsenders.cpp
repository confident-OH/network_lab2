#include "stdafx.h"
#include "Global.h"
#include "SRsenders.h"

SRsenders::SRsenders()
    :base(0),nextseqnum(0)
{
	int i, j;
	for (i = 0; i < SEQUN; i++) {
		this->label[i] = false;
		for (j = 0; j < Configuration::PAYLOAD_SIZE; j++) {
			this->packetWaitingAck[i].payload[i] = '.';
		}
		this->packetWaitingAck[i].acknum = -1;
		this->packetWaitingAck[i].checksum = 0;
		this->packetWaitingAck[i].seqnum = -1;
	}
}

SRsenders::~SRsenders() 
{

}

bool SRsenders::getWaitingState()
{
	return !((nextseqnum + SEQUN - base) % SEQUN < WIDEN);
}

bool SRsenders::send(const Message& message) {
	if (getWaitingState()) { //发送方处于等待确认状态
		return false;
	}
	else{
		//在窗口中设置ACKS
		std::cout << "\n[SENDER]发送前窗口：";
		printSlideWindow();
		this->packetWaitingAck[nextseqnum].acknum = -1; //忽略该字段
		this->packetWaitingAck[nextseqnum].seqnum = this->nextseqnum;
		this->packetWaitingAck[nextseqnum].checksum = 0;
		memcpy(this->packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
		this->packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextseqnum]);
		pUtils->printPacket("发送方发送报文", this->packetWaitingAck[nextseqnum]);
		pns->startTimer(SENDER, Configuration::TIME_OUT, this->nextseqnum);			//启动发送方定时器
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
		this->nextseqnum = (this->nextseqnum + 1) % SEQUN;
		std::cout << "[SENDER]发送后窗口：";
		printSlideWindow();
		cout << endl;
		return true;
	}
}

void SRsenders::receive(const Packet& ackPkt) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	//如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
	if (checkSum != ackPkt.checksum) {
		pUtils->printPacket("\n[Debug]数据校验错误-接收的ack损坏", ackPkt);
	}
	else {
		int num = ackPkt.acknum;
		if (num == this->base) {  
			//pUtils->printPacket("发送方正确收到确认", ackPkt);       
			cout << "\n[SENDER]收到ack:" << ackPkt.acknum << "，移动滑动窗口：";   //准备改变滑动窗格
			label[base] = true;
			pns->stopTimer(SENDER, ackPkt.acknum);                      //关闭定时器
			while (this->label[this->base]) {                             	
				label[base] = false;
				this->base = (this->base + 1) % SEQUN;
			}
			cout << "\n[SENDER]收到ack，滑动窗口移动：";
			printSlideWindow();
			cout << endl;
		}
		else {
			if ((SEQUN + num - this->base) % SEQUN < (SEQUN + this->nextseqnum - this->base) % SEQUN) {
				pns->stopTimer(SENDER, ackPkt.acknum);	                                	//关闭定时器
				label[num] = true;
				cout << "\n[SENDER]收到ack:" << ackPkt.acknum << endl;
				pUtils->printPacket("发送方缓存正确收到确认", this->packetWaitingAck[ackPkt.acknum]);
			}
		}
	}	
}


void SRsenders::timeoutHandler(int seqNum)
{
	pUtils->printPacket("[ERROR]发送超时", this->packetWaitingAck[seqNum]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[seqNum]);
	pUtils->printPacket("[Debug]超时重传的分组", packetWaitingAck[seqNum]);
}

void SRsenders::printSlideWindow()
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