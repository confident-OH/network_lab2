#include "stdafx.h"
#include "GBNreceiver.h"
#include "Global.h"

GBNreceiver::GBNreceiver()
{
	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ0��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ0
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	memset(lastAckPkt.payload, '.', Configuration::PAYLOAD_SIZE);
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
    this->expectedseq = 0;
}

GBNreceiver::~GBNreceiver()
{

}

void GBNreceiver::receive(const Packet& packet)
{
	if (packet.checksum != pUtils->calculateCheckSum(packet))    //���ݰ���
	{
		pUtils->printPacket("[Debug]����У�����-���շ�û����ȷ���ձ���", packet);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);             //�����ϴε�
		return;
	}
	if (packet.seqnum != this->expectedseq)                      //������Ҫ�����ݰ���������Ӧ��
	{
		pUtils->printPacket("[Debug]�������������ݷ���", packet);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);             //������ǰ�����ݰ�
		return;
	}
	else
	{
		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);
		//pUtils->printPacket("\n��ȷ���ܲ����͵�Ӧ�ò㣺", packet);
		lastAckPkt.acknum = packet.seqnum;                       //ȷ����ŵ����յ��ı������
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		//pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
		this->expectedseq = (this->expectedseq + 1) % SEQUN;//������ȡģ
	}
	cout << "\n[RECEIVER]ȷ�Ϻ�ack��" << lastAckPkt.acknum << "\n\n";
}