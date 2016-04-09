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

RT_MUTEX mutexEtat;
RT_MUTEX mutexMove;
RT_MUTEX mutexWatchdog;
RT_MUTEX mutexRobot;

RT_SEM semConnecterRobot;
RT_SEM semStartRobot;
RT_SEM semWatchdog;
RT_SEM semWatchdog2;



RT_QUEUE queueMsgGUI;

int etatCommMoniteur = 1;
int etatCommRobot = 1;
int failsCommRobot = 0;
int enableImageAcquisition = 1;
int watchdogReset = 1;

DRobot *robot;
DMovement *move;
DServer *serveur;


int MSG_QUEUE_SIZE = 10;

int PRIORITY_TSERVEUR = 30;
int PRIORITY_TCONNECT = 20;
int PRIORITY_TMOVE = 15;
int PRIORITY_TENVOYER = 25;
int PRIORITY_TIMAGEACQ = 11;
int PRIORITY_TWATCHDOG = 23;
int PRIORITY_TBATTERY = 24;
