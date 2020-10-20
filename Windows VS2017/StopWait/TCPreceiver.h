#pragma once
#include "RdtReceiver.h"
#define SEQUN 8
#define WIDEN 4

class TCPreceiver 
    :public RdtReceiver
{
private:
    int base;
public:
    TCPreceiver();
    virtual ~TCPreceiver();
};