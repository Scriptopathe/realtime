/*
 * File:   global.h
 * Author: pehladik
 *
 * Created on 21 avril 2011, 12:14
 */

#include "global.h"

RT_TASK tServeur;
RT_TASK tconnect;
RT_TASK tmove;
RT_TASK tenvoyer;
RT_TASK tImageAcquisition;
RT_TASK tBattery;
RT_TASK tArena;
RT_TASK tPosition;

RT_MUTEX mutexEtat;
RT_MUTEX mutexMove;
RT_MUTEX mutexWatchdog;
RT_MUTEX mutexRobot;
RT_MUTEX mutexImage;
RT_MUTEX mutexArena;
RT_MUTEX mutexPosCompute;

RT_SEM semConnecterRobot;
RT_SEM semStartRobot;
RT_SEM semWatchdog;
RT_SEM semWatchdog2;
RT_SEM semArena;



RT_QUEUE queueMsgGUI;

int etatCommMoniteur = 1;
int etatCommRobot = 1;
int failsCommRobot = 0;
int enableImageAcquisition = 1;
int watchdogReset = 1;
int findingArena = 0;
int enablePosCompute = 0;

DRobot *robot;
DMovement *move;
DServer *serveur;
DCamera *camera;
DImage *image;
DArena *arena;


int MSG_QUEUE_SIZE = 10;

int PRIORITY_TSERVEUR = 30;
int PRIORITY_TCONNECT = 20;
int PRIORITY_TMOVE = 15;
int PRIORITY_TENVOYER = 25;
int PRIORITY_TIMAGEACQ = 11;
int PRIORITY_TWATCHDOG = 23;
int PRIORITY_TBATTERY = 24;
int PRIORITY_TARENA = 21;
int PRIORITY_TPOSITION = 22;

void setImage(DImage* img)
{
   rt_mutex_acquire(&mutexImage, TM_INFINITE);
   memcpy(image, img, sizeof(DImage)); 
   rt_mutex_release(&mutexImage);
}

int isFindingArena()
{
    int value;
    rt_mutex_acquire(&mutexArena, TM_INFINITE);
    value = findingArena;
    rt_mutex_release(&mutexArena);
    return value;
}

void setFindingArena(int value)
{
    rt_mutex_acquire(&mutexArena, TM_INFINITE);
    findingArena = value;
    rt_mutex_release(&mutexArena);
}

int getMonitorStatus()
{
    int comStatus;
    rt_mutex_acquire(&mutexEtat, TM_INFINITE);
    comStatus = etatCommMoniteur;
    rt_mutex_release(&mutexEtat);
    return comStatus;
}

int posComputeEnabled()
{
    int value;
    rt_mutex_acquire(&mutexPosCompute, TM_INFINITE);
    value = enablePosCompute;
    rt_mutex_release(&mutexPosCompute);
    return value;
}

void setPosComputeEnabled(int value)
{
    rt_mutex_acquire(&mutexPosCompute, TM_INFINITE);
    enablePosCompute = value;
    rt_mutex_release(&mutexPosCompute);
}


