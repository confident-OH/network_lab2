#include "stdafx.h"
#include "GBNreceiver.h"
#include "Global.h"

GBNreceiver::GBNreceiver()
{
	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为0，使得当第一个接受的数据包出错时该确认报文的确认号为0
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
	memset(lastAckPkt.payload, '.', Configuration::PAYLOAD_SIZE);
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
    this->expectedseq = 0;
}

GBNreceiver::~GBNreceiver()
{

}

void GBNreceiver::receive(const Packet& packet)
{
	if (packet.checksum != pUtils->calculateCheckSum(packet))    //数据包损坏
	{
		pUtils->printPacket("[Debug]数据校验错误-接收方没有正确接收报文", packet);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);             //发送上次的
		return;
	}
	if (packet.seqnum != this->expectedseq)                      //不是想要的数据包，不作出应答
	{
		pUtils->printPacket("[Debug]不是期望的数据分组", packet);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);             //发送以前的数据包
		return;
	}
	else
	{
		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);
		//pUtils->printPacket("\n正确接受并发送到应用层：", packet);
		lastAckPkt.acknum = packet.seqnum;                       //确认序号等于收到的报文序号
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		//pUtils->printPacket("接收方发送确认报文", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
		this->expectedseq = (this->expectedseq + 1) % SEQUN;//别忘了取模
	}
	cout << "\n[RECEIVER]确认号ack：" << lastAckPkt.acknum << "\n\n";
}