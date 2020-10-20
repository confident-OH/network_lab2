#include "stdafx.h"
#include "Global.h"
#include "TCPsender.h"

TCPsender::TCPsender()
    :base(0), nextseqnum(0)
{
    memset(this->packetWaitingAck, 0, sizeof(this->packetWaitingAck));
}

TCPsender::~TCPsender()
{

}

bool TCPsender::getWaitingState()
{
    return !((nextseqnum + SEQUN - base) % SEQUN < WIDEN);
}

bool TCPsender::send(Message& message)
{

}