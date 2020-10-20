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
	if (getWaitingState()) { //���ͷ����ڵȴ�ȷ��״̬
		std::cout << "�ڵȴ�������" << endl;
		return false;
	}
	else
	{
		//�ڴ���������ACKS
		this->waitingState = true;
		this->packetWaitingAck[nextseqnum].acknum = -1; //���Ը��ֶ�
		this->packetWaitingAck[nextseqnum].seqnum = this->nextseqnum;
		this->packetWaitingAck[nextseqnum].checksum = 0;
		memcpy(this->packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
		this->packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextseqnum]);
		pUtils->printPacket("���ͷ����ͱ���", this->packetWaitingAck[nextseqnum]);
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
		if(base == nextseqnum)
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWaitingAck[nextseqnum].seqnum);			//�������ͷ���ʱ��
		this->nextseqnum = (this->nextseqnum + 1) % SEQUN;
		return true;
    }
}

void GBNsenders::receive(const Packet& ackPkt) {
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum != ackPkt.checksum)
	{
		pUtils->printPacket("\n[Debug]���յ�ack�𻵣�У��Ͳ����", ackPkt);
	}
	else
	{
		base = (ackPkt.acknum + 1) % SEQUN;//�ۻ�ȷ�ϣ�������ȡģ
		if (this->base == this->nextseqnum)
		{
			pns->stopTimer(SENDER, 0);
		}
		else
		{//������ʱ��
			pns->stopTimer(SENDER, 0);
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		}
		std::cout << "\n[SENDER]�յ�ack�����������ƶ���";
		printSlideWindow();
		std::cout << endl;
	}
}

void GBNsenders::timeoutHandler(int seqNum)
{
	int i;
	std::cout << "\n���ֳ�ʱ" << endl;
	if (this->nextseqnum == this->base)
	{
		//��ʱ��������������
		return;
	}
	else {
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
		for (i = this->base; i != this->nextseqnum; i = (i + 1) % SEQUN) {
			pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[i]);
			pUtils->printPacket("[Debug]��ʱ�ش��ķ���:", packetWaitingAck[i]);
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
		std::cout << i;
		if (i == this->nextseqnum)
			std::cout << "|";
		if (i == (base + WIDEN - 1) % SEQUN)
			std::cout << "]";
		std::cout << " ";
	}
	std::cout << std::endl;
}