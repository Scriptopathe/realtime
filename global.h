/* 
 * File:   global.h
 * Author: pehladik
 *
 * Created on 12 janvier 2012, 10:11
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#include "includes.h"

/* @descripteurs des tâches */
extern RT_TASK tServeur;
extern RT_TASK tconnect;
extern RT_TASK tmove;
extern RT_TASK tenvoyer;
extern RT_TASK tImageAcquisition;
extern RT_TASK tWatchdog;
extern RT_TASK tBattery;


/* @descripteurs des mutex */
extern RT_MUTEX mutexEtat;
extern RT_MUTEX mutexRobot;
extern RT_MUTEX mutexMove;
extern RT_MUTEX mutexWatchdog;
extern RT_MUTEX mutexImage;
extern RT_MUTEX mutexArena;

/* @descripteurs des sempahore */
extern RT_SEM semConnecterRobot;
extern RT_SEM semStartRobot;
extern RT_SEM semWatchdog;
extern RT_SEM semWatchdog2;
extern RT_SEM semArena;

/* @descripteurs des files de messages */
extern RT_QUEUE queueMsgGUI;

/* @variables partagées */
extern int etatCommMoniteur;
extern int etatCommRobot;
extern int failsCommRobot;
extern int enableImageAcquisition;
extern int watchdogReset;
extern int findingArena;


extern DServer *serveur;
extern DRobot *robot;
extern DMovement *move;
extern DCamera *camera;
extern DImage *image;
extern DArena *arena;

/* @constantes */
extern int MSG_QUEUE_SIZE;
extern int PRIORITY_TSERVEUR;
extern int PRIORITY_TCONNECT;
extern int PRIORITY_TMOVE;
extern int PRIORITY_TENVOYER;
extern int PRIORITY_TIMAGEACQ;
extern int PRIORITY_TWATCHDOG;
extern int PRIORITY_TBATTERY;

void setImage(DImage* image);
int isFindingArena();
void setFindingArena(int value);
int getMonitorStatus();
#endif	/* GLOBAL_H */

