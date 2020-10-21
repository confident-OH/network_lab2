// StopWait.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "GBNsender.h"
#include "GBNreceiver.h"
#include "SRreceiver.h"
#include "SRsenders.h"
#include "TCPs.h"

FILE* fp;

int main(int argc, char* argv[])
{
	
	//RdtSender* gs = new GBNsenders();
	//RdtReceiver* gr = new GBNreceiver();
	//fp = fopen("GBNwindow.txt", "w");

	//RdtSender* gs = new SRsenders();
	//RdtReceiver* gr = new SRreceiver();
	//fp = fopen("SRwindow.txt", "w");

	RdtSender* gs = new TCPsenders();
	RdtReceiver* gr = new GBNreceiver();
	fp = fopen("TCPwindow.txt", "w");

	//	pns->setRunMode(0);  //VERBOS模式
	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(gs);
	pns->setRtdReceiver(gr);
	pns->setInputFile("E:\\network_lab\\LAB2\\Windows VS2017\\input.txt");
	pns->setOutputFile("output.txt");

	pns->start();

	delete gs;
	delete gr;
	delete pUtils;		//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;			//指向唯一的模拟网络环境类实例，只在main函数结束前delete

	return 0;
}

