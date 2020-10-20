#include "stdafx.h"
#include "SRreceiver.h"
#include "Global.h"

SRreceiver::SRreceiver()
	:expectebase(0)
{
	int i;
	for (i = 0; i < SEQUN; i++) {
		this->isrvc[i] = false;
		memset(this->massn[i].data, 0, sizeof(this->massn[i].data));
	}
	this->lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	this->lastAckPkt.checksum = 0;
	this->lastAckPkt.seqnum = 0;	//���Ը��ֶ�
	for (i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		this->lastAckPkt.payload[i] = '.';
	}
	this->lastAckPkt.checksum = pUtils->calculateCheckSum(this->lastAckPkt);
}

SRreceiver::~SRreceiver()
{

}

void SRreceiver::receive(const Packet& packet)
{
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(packet);
	int rvsequ = packet.seqnum;
	//���У�����ȷ��ͬʱ�յ����ĵ���ŵ��ڽ��շ��ڴ��յ��ı������һ��
	if (checkSum != packet.checksum) {
		pUtils->printPacket("[Debug]���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
	}
	else {
		if (this->expectebase == rvsequ) {
			pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);
			this->isrvc[rvsequ] = true;
			//ȡ��Message�����ϵݽ���Ӧ�ò�
			memcpy(this->massn[rvsequ].data, packet.payload, sizeof(packet.payload));

			this->lastAckPkt.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
			this->lastAckPkt.checksum = pUtils->calculateCheckSum(this->lastAckPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", this->lastAckPkt);
			pns->sendToNetworkLayer(SENDER, this->lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�	

			while (this->isrvc[this->expectebase] == true) {
				std::cout << "��ʼ�ϴ�" << endl;
				pns->delivertoAppLayer(RECEIVER, this->massn[expectebase % SEQUN]);
				this->isrvc[this->expectebase % SEQUN] = false;
				this->expectebase = (this->expectebase + 1) % SEQUN;
			}

		}
		else {
			if ((rvsequ + SEQUN - expectebase)%SEQUN < WIDEN) {
				memcpy(this->massn[rvsequ].data, packet.payload, sizeof(packet.payload));
				this->lastAckPkt.acknum = rvsequ; //ȷ����ŵ����յ��ı������
				this->lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
				isrvc[rvsequ] = true;
				pUtils->printPacket("ʧ��ı��ģ��÷��ͷ�����", this->lastAckPkt);
				pns->sendToNetworkLayer(SENDER, this->lastAckPkt);	               //���ͱ����÷��ͻ�����
			}
			else {
				pUtils->printPacket("[Debug]����δ�ڴ�����", packet);
				this->lastAckPkt.acknum = rvsequ;                                  //ȷ����ŵ����յ��ı������
				this->lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
				pns->sendToNetworkLayer(SENDER, this->lastAckPkt);	               //�������������ش�            
			}

		}
	}
	
}