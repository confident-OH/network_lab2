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
	if (getWaitingState()) { //���ͷ����ڵȴ�ȷ��״̬
		std::cout << "�ڵȴ�������" << endl;
		return false;
	}
	else
	{
		
		std::cout << "\n[SENDER]����ǰ���ڣ�";
		fprintf(fp, "\n[SENDER]����ǰ���ڣ�");
		printSlideWindow();
		this->packetWaitingAck[nextseqnum].acknum = -1;                  //���Ը��ֶ�
		this->packetWaitingAck[nextseqnum].seqnum = this->nextseqnum;
		this->packetWaitingAck[nextseqnum].checksum = 0;
		memcpy(this->packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
		this->packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextseqnum]);
		pUtils->printPacket("���ͷ����ͱ���", this->packetWaitingAck[nextseqnum]);
		fprintf(fp, "���ͷ����ͱ���: %s", this->packetWaitingAck[nextseqnum].payload);
		pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
		if (base == nextseqnum)
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->nextseqnum);			//�������ͷ���ʱ��
		this->nextseqnum = (this->nextseqnum + 1) % SEQUN;

		std::cout << "[SENDER]���ͺ󴰿ڣ�";
		fprintf(fp, "[SENDER]���ͺ󴰿ڣ�");
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
		pUtils->printPacket("\n[Debug]����У�����-���յ�ack��", ackPkt);
	}
	else
	{
		if ((sum+SEQUN-this->base)%SEQUN < WIDEN)    //�ڻ��������ڣ��ƶ���������
		{
			pns->stopTimer(SENDER, base);
			base = (ackPkt.acknum + 1) % SEQUN;
			
			if (base != this->nextseqnum)
			{
				pns->startTimer(SENDER, Configuration::TIME_OUT, base);
			}
			this->acksacc = 0;
			cout << "\n[SENDER]�յ�ack:" << ackPkt.acknum << "���ƶ��������ڣ�";
			fprintf(fp, "\n[SENDER]�յ�ack:%d���ƶ��������ڣ�", ackPkt.acknum);
			printSlideWindow();
			cout << endl;
			fprintf(fp, "\n");
		}
		else
		{      //���ܵ������ack
			this->acksacc = (this->acksacc + 1) % 3;
			if (this->acksacc == 2)      //�����ش�
			{
				std::cout << "\n[ERROR]�յ�������������ack�������ش�\n";
				pUtils->printPacket("�����ش�", this->packetWaitingAck[base]);
				fprintf(fp, "\n[ERROR]�յ�������������ack�������ش�:%s", this->packetWaitingAck[base].payload);
				pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[base]);
				
			}
		}
		
	}
}

void TCPsenders::timeoutHandler(int seqNum)
{
	std::cout << "\n[ERROR]���ֳ�ʱ" << endl;
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[base]);
	pUtils->printPacket("[Debug]��ʱ�ش��ķ���", packetWaitingAck[base]);
}

void TCPsenders::printSlideWindow()
{
	int i;
	for (i = 0; i < SEQUN; i++)
	{
		if (i == this->base) {
			std::cout << "[";
			fprintf(fp, "[");
		}
			
		if (i == this->nextseqnum) {
			std::cout << "|";
			fprintf(fp, "|");
		}
		std::cout << i;
		fprintf(fp, "%d", i);
		if (i == (base + WIDEN - 1) % SEQUN) {
			std::cout << "]";
			fprintf(fp, "]");
		}
		fprintf(fp, " ");
		std::cout << " ";
	}
	std::cout << std::endl;
	fprintf(fp, "\n");
}