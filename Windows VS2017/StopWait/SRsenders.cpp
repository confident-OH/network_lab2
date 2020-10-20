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
	if (getWaitingState()) { //���ͷ����ڵȴ�ȷ��״̬
		return false;
	}
	else{
		//�ڴ���������ACKS
		std::cout << "\n[SENDER]����ǰ���ڣ�";
		printSlideWindow();
		this->packetWaitingAck[nextseqnum].acknum = -1; //���Ը��ֶ�
		this->packetWaitingAck[nextseqnum].seqnum = this->nextseqnum;
		this->packetWaitingAck[nextseqnum].checksum = 0;
		memcpy(this->packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
		this->packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextseqnum]);
		pUtils->printPacket("���ͷ����ͱ���", this->packetWaitingAck[nextseqnum]);
		pns->startTimer(SENDER, Configuration::TIME_OUT, this->nextseqnum);			//�������ͷ���ʱ��
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
		this->nextseqnum = (this->nextseqnum + 1) % SEQUN;
		std::cout << "[SENDER]���ͺ󴰿ڣ�";
		printSlideWindow();
		cout << endl;
		return true;
	}
}

void SRsenders::receive(const Packet& ackPkt) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	//���У�����ȷ������ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
	if (checkSum != ackPkt.checksum) {
		pUtils->printPacket("\n[Debug]����У�����-���յ�ack��", ackPkt);
	}
	else {
		int num = ackPkt.acknum;
		if (num == this->base) {  
			//pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);       
			cout << "\n[SENDER]�յ�ack:" << ackPkt.acknum << "���ƶ��������ڣ�";   //׼���ı们������
			label[base] = true;
			pns->stopTimer(SENDER, ackPkt.acknum);                      //�رն�ʱ��
			while (this->label[this->base]) {                             	
				label[base] = false;
				this->base = (this->base + 1) % SEQUN;
			}
			cout << "\n[SENDER]�յ�ack�����������ƶ���";
			printSlideWindow();
			cout << endl;
		}
		else {
			if ((SEQUN + num - this->base) % SEQUN < (SEQUN + this->nextseqnum - this->base) % SEQUN) {
				pns->stopTimer(SENDER, ackPkt.acknum);	                                	//�رն�ʱ��
				label[num] = true;
				cout << "\n[SENDER]�յ�ack:" << ackPkt.acknum << endl;
				pUtils->printPacket("���ͷ�������ȷ�յ�ȷ��", this->packetWaitingAck[ackPkt.acknum]);
			}
		}
	}	
}


void SRsenders::timeoutHandler(int seqNum)
{
	pUtils->printPacket("[ERROR]���ͳ�ʱ", this->packetWaitingAck[seqNum]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[seqNum]);
	pUtils->printPacket("[Debug]��ʱ�ش��ķ���", packetWaitingAck[seqNum]);
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