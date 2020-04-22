#include "planificador.h"

void inicializarPid() {
    pid = 0;
}

uint32_t getNuevoPid() {
    uint32_t nuevoPid = pid++;

    return nuevoPid;
}