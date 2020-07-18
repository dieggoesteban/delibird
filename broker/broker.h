#ifndef BROKER_H_
#define BROKER_H_

#include "global-includes.h"
#include "models.h"
#include "sockets.h"
#include "messageQueueManager.h"
#include "cache.h"

void terminarPrograma();

sem_t waitForFinish;
pthread_t t_finishProgram;

#endif /* BROKER_H_ */