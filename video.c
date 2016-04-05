#include "video.h"


void computeArena(void* arg)
{

}

void computePosition(void* arg)
{

}

void acquireImage(void* arg)
{
    rt_task_set_periodic(NULL, TM_NOW, 600e+6);

    while (1) {
        /* Attente de l'activation périodique */
        rt_task_wait_period(NULL);
        rt_printf("tImageAcquisition : Activation périodique\n");
    }

}
