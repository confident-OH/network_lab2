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
	this->lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	this->lastAckPkt.checksum = 0;
	this->lastAckPkt.seqnum = 0;	//忽略该字段
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
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(packet);
	int rvsequ = packet.seqnum;
	//如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
	if (checkSum != packet.checksum) {
		pUtils->printPacket("[Debug]接收方没有正确收到发送方的报文,数据校验错误", packet);
	}
	else {
		if (this->expectebase == rvsequ) {
			pUtils->printPacket("接收方正确收到发送方的报文", packet);
			this->isrvc[rvsequ] = true;
			//取出Message，向上递交给应用层
			memcpy(this->massn[rvsequ].data, packet.payload, sizeof(packet.payload));

			this->lastAckPkt.acknum = packet.seqnum; //确认序号等于收到的报文序号
			this->lastAckPkt.checksum = pUtils->calculateCheckSum(this->lastAckPkt);
			pUtils->printPacket("接收方发送确认报文", this->lastAckPkt);
			pns->sendToNetworkLayer(SENDER, this->lastAckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方	

			while (this->isrvc[this->expectebase] == true) {
				std::cout << "开始上传" << endl;
				pns->delivertoAppLayer(RECEIVER, this->massn[expectebase % SEQUN]);
				this->isrvc[this->expectebase % SEQUN] = false;
				this->expectebase = (this->expectebase + 1) % SEQUN;
			}

		}
		else {
			if ((rvsequ + SEQUN - expectebase)%SEQUN < WIDEN) {
				memcpy(this->massn[rvsequ].data, packet.payload, sizeof(packet.payload));
				this->lastAckPkt.acknum = rvsequ; //确认序号等于收到的报文序号
				this->lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
				isrvc[rvsequ] = true;
				pUtils->printPacket("失序的报文，让发送方缓存", this->lastAckPkt);
				pns->sendToNetworkLayer(SENDER, this->lastAckPkt);	               //发送报文让发送机缓存
			}
			else {
				pUtils->printPacket("[Debug]报文未在窗口中", packet);
				this->lastAckPkt.acknum = rvsequ;                                  //确认序号等于收到的报文序号
				this->lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
				pns->sendToNetworkLayer(SENDER, this->lastAckPkt);	               //无论怎样，都重传            
			}

		}
	}
	
}