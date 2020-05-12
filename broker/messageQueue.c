#include "messageQueue.h"


void inicializarCounterMessageId()
{
    counterMessageId = 0;
}

uint32_t asignarMessageId()
{
    return ++counterMessageId;
}

t_list* suscriptoresCola(uint32_t mq_cod){
    t_message_queue* MQ = getMessageQueueById(mq_cod);
    t_list* suscriptores = list_create();
    for(uint32_t i = 0; i < list_size(MQ->suscribers); i++){
        list_add(suscriptores, list_get(MQ->suscribers,i));
    }
    return suscriptores;
}

// //PARA CUANDO VIENE UN NUEVO MODULO Y SE SUSCRIBE - ES CACHE
// void dispatcArchivedhMessages(t_message_queue* messageQueue){
//     uint32_t messagesCount = list_size(messageQueue->mensajes);
//     uint32_t suscribersCount = list_size(messageQueue->suscribers);
//     void* currentMessageToSend;
//     uint32_t currentSuscriberToSend;
//     for(uint32_t i = 0; i < messagesCount; i++){
//         currentMessageToSend = list_get(messageQueue->mensajes, i);
//         for(uint32_t j = 0; j < suscribersCount; j++){
//             currentSuscriberToSend = list_get(messageQueue->suscribers, j);
//             //SEND
//         }
//     }
// }


void dispatchMessage(uint32_t mq_cod, t_paquete* paquete){
    t_list* suscriptores = suscriptoresCola(mq_cod);
    for(uint32_t i = 0; i < list_size(suscriptores); i++){
        enviarMensaje(paquete, (uint32_t)list_get(suscriptores,i));
    }

}




//Aplica el ID al mensaje y lo prepara para enviar
// void* prepareMessageToSend() {

// }

// void dispatchMessages(t_message_queue* messageQueue)
// {
//     uint32_t messagesCount = list_size(messageQueue->mensajes);
//     uint32_t suscribersCount = list_size(messageQueue->suscribers);
//     void* currentMessageToSend;
//     uint32_t* currentSuscriberToSend;
//     for (uint32_t i = 0; i < messagesCount; i++) {
//         currentMessageToSend = list_get(messageQueue->mensajes, i);
//         for (uint32_t j = 0; j < suscribersCount; j++) {
//             currentSuscriberToSend = list_get(messageQueue->suscribers, j);
//             sendMessage(currentMessageToSend, currentSuscriberToSend);
//         } 
//     }
// }

// void sendMessage(void* message, uint32_t idTargetModule) {
//     //TODO: Send the message 
// }