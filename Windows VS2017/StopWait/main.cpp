// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "GBNsender.h"
#include "GBNreceiver.h"
#include "SRreceiver.h"
#include "SRsenders.h"


int main(int argc, char* argv[])
{
	//RdtSender* gs = new GBNsenders();
	//RdtReceiver* gr = new GBNreceiver();

	RdtSender* gs = new SRsenders();
	RdtReceiver* gr = new SRreceiver();
	
	//	pns->setRunMode(0);  //VERBOSģʽ
	pns->setRunMode(1);  //����ģʽ
	pns->init();
	pns->setRtdSender(gs);
	pns->setRtdReceiver(gr);
	pns->setInputFile("E:\\network_lab\\LAB2\\Windows VS2017\\input.txt");
	pns->setOutputFile("output.txt");

	pns->start();

	delete gs;
	delete gr;
	delete pUtils;		//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;			//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete

	return 0;
}

