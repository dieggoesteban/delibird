#include "sockets.h"

//Aplica el ID al mensaje y lo prepara para enviar
/*void* prepareMessageToSend() {

}

void dispatchMessages(t_message_queue* messageQueue)
{
    uint32_t messagesCount = list_size(messageQueue->mensajes);
    uint32_t suscribersCount = list_size(messageQueue->suscribers);
    void* currentMessageToSend;
    uint32_t* currentSuscriberToSend;
    for (uint32_t i = 0; i < messagesCount; i++) {
        currentMessageToSend = list_get(messageQueue->mensajes, i);
        for (uint32_t j = 0; j < suscribersCount; j++) {
            currentSuscriberToSend = list_get(messageQueue->suscribers, j);
            sendMessage(currentMessageToSend, currentSuscriberToSend);
        } 
    }
}

void sendMessage(void* message, uint32_t idTargetModule) {
    //TODO: Send the message 
}*/ 