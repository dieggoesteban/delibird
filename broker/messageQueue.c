#include "messageQueue.h"

void inicializarCounterMessageId()
{
    counterMessageId = 0;
}

uint32_t asignarMessageId()
{
    return ++counterMessageId;
}