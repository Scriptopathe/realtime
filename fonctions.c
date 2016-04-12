#include "fonctions.h"

#define EN_WD 0
int write_in_queue(RT_QUEUE *msgQueue, void * data, int size);

/**
 * Vérifie le statut renvoyé par une commande au robot.
 * Au bout de 3 échecs consécutifs, l'état de la connexion
 * passe en erreur et un message est envoyé au moniteur.
 */
int check_status(int status)
{
    printf("check_status: %d\n", status);
    DMessage *message;
    rt_mutex_acquire(&mutexEtat, TM_INFINITE);
    int oldStatus = etatCommRobot;
    if(status == STATUS_OK)
    {
        failsCommRobot = 0;
        etatCommRobot = STATUS_OK;
    }
    else
    {
        printf("echec de connexion (%d/3)\n", failsCommRobot+1);
        if(failsCommRobot++ >= 3)
        {
            printf("Connexion perdue... reconnexion...\n");
            etatCommRobot = status;
            // rt_sem_v(&semConnecterRobot);
        }
    }
    // Envoi du message au moniteur
    if(oldStatus != etatCommRobot)
    {
         message = d_new_message();
         message->put_state(message, status);

         printf("check_status : Envoi message\n");
         if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0) 
         {
            // message->free(message);
         }
         
    }
    rt_mutex_release(&mutexEtat);
    return status;
}


void watchdog(void* arg)
{
#if !EN_WD
    return;
#endif
    int reset;
    
    while (1) 
    {
        // On attend le signal de lancement du watchdog
        rt_sem_v(&semWatchdog2);
        rt_sem_p(&semWatchdog, TM_INFINITE);

        // On remet reset la période.
        rt_task_set_periodic(NULL, TM_NOW, 1000e+6); // 1sec

        reset = 0;
        while(!reset)
        {
       
            // Appel du reload.
            rt_mutex_acquire(&mutexRobot, TM_INFINITE);
            robot->reload_wdt(robot);
            rt_mutex_release(&mutexRobot);

            // On check le status du watchdog
            rt_mutex_acquire(&mutexWatchdog, TM_INFINITE);
            reset = watchdogReset;
            rt_mutex_release(&mutexWatchdog);

            /* Attente de l'activation périodique */
            rt_task_wait_period(NULL);

        }
    }
}

void envoyer(void * arg) {
    DMessage *msg;
    int err;

    while (1) {
        printf("tenvoyer : Attente d'un message\n");
        if ((err = rt_queue_read(&queueMsgGUI, &msg, sizeof (DMessage), TM_INFINITE)) >= 0) 
        {
            printf("tenvoyer : envoi d'un message au moniteur\n");
            serveur->send(serveur, msg);
            msg->free(msg);
        } else {
            printf("Error msg queue write: %s\n", strerror(-err));
        }
    }
}

void check_battery(void * arg)
{
    int batLevel, status;
    
    // Période : 250ms
    rt_task_set_periodic(NULL, TM_NOW, 250e+6);

    while(1)
    {
        rt_task_wait_period(NULL);

        // Si le robot n'est pas connecté, on attend la prochaine
        // activation périodique pour retester.
        rt_mutex_acquire(&mutexEtat, TM_INFINITE);
        status = etatCommRobot;
        rt_mutex_release(&mutexEtat);

        if(status != STATUS_OK)
            continue;

        // Status
        rt_mutex_acquire(&mutexRobot, TM_INFINITE);
        status = robot->get_vbat(robot, &batLevel);
        rt_mutex_release(&mutexRobot);

        DBattery * battery = d_new_battery();
        battery->set_level(battery, batLevel);
        
        if(check_status(status) == STATUS_OK)
        {
             DMessage* message = d_new_message();
             message->put_battery_level(message, battery);

             printf("check_battery : Envoi message\n");
             if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0)
             {
                // message->free(message);
             }
        }
    }

}

void connecter(void * arg) {
    int status;
    printf("tconnect : Debut de l'exécution de tconnect\n");

    while (1) {
        printf("tconnect : Attente du sémarphore semConnecterRobot\n");
        rt_sem_p(&semConnecterRobot, TM_INFINITE);
        printf("tconnect : Ouverture de la communication avec le robot\n");
        status = robot->open_device(robot);

        printf("tconnect: Open Device Status = %d\n", status);
        
        if(check_status(status) == STATUS_OK)
        {
#if EN_WD
            // On démarre le watchdog
            rt_mutex_acquire(&mutexWatchdog, TM_INFINITE);
            watchdogReset = 1;
            rt_mutex_release(&mutexWatchdog);
            
            // On attend que le watchdog soit sorti de sa boucle.
            rt_sem_p(&semWatchdog2, TM_INFINITE);
            
            // On démarre le robot une fois le watchdog complètement 
            // démarré.
            status = robot->start(robot);
            printf("tconnect: Start Status = %d\n", status);
            if (check_status(status) == STATUS_OK){
                printf("tconnect : Robot démarré\n");
            }

            // On lance le watchdog
            rt_sem_v(&semWatchdog);
#else
            // Version sans watchdog
            status = robot->start_insecurely(robot);
            printf("tconnect: Start Status = %d\n", status);
            if (check_status(status) == STATUS_OK){
                printf("tconnect : Robot démarré\n");
            }
#endif
        }
    }
}

void communiquer(void *arg) {
    DMessage *msg = d_new_message();
    int var1 = 1;
    int num_msg = 0;

    printf("tserver : Début de l'exécution de serveur\n");
    serveur->open(serveur, "8000");
    printf("tserver : Connexion\n");

    rt_mutex_acquire(&mutexEtat, TM_INFINITE);
    etatCommMoniteur = 0;
    rt_mutex_release(&mutexEtat);

    while (var1 > 0) {
        printf("tserver : Attente d'un message\n");
        var1 = serveur->receive(serveur, msg);
        num_msg++;
        if (var1 > 0) {
            switch (msg->get_type(msg)) {
                case MESSAGE_TYPE_ACTION:
                    printf("tserver : Le message %d reçu est une action\n",
                            num_msg);
                    DAction *action = d_new_action();
                    action->from_message(action, msg);
                    switch (action->get_order(action)) {
                        case ACTION_CONNECT_ROBOT:
                            printf("tserver : Action connecter robot\n");
                            rt_sem_v(&semConnecterRobot);
                            break;
                        case ACTION_FIND_ARENA:
                            printf("tserver : Action find arena\n");
                            rt_sem_v(&semArena);
                            break;
                        case ACTION_ARENA_IS_FOUND:
                            printf("tserver : Action arena is found\n");

                            // On remplace l'ancienne par la nouvelle.
                            rt_mutex_acquire(&mutexArena, TM_INFINITE);
                            free(arena);
                            arena = tmpArena;
                            rt_mutex_release(&mutexArena);

                            setFindingArena(0);
                            break;
                        case ACTION_ARENA_FAILED:
                            printf("tserver : Action arena failed\n");
                            setFindingArena(0);

                            break;
                        case ACTION_COMPUTE_CONTINUOUSLY_POSITION:
                            printf("tserver: Compute position\n");
                            setPosComputeEnabled(1);
                            break;
                    }

                    break;
                case MESSAGE_TYPE_MOVEMENT:
                    printf("tserver : Le message reçu %d est un mouvement\n",
                            num_msg);
                    rt_mutex_acquire(&mutexMove, TM_INFINITE);
                    move->from_message(move, msg);
                    move->print(move);
                    rt_mutex_release(&mutexMove);
                    break;
            }
        }
    }
}

void deplacer(void *arg) {
    int status = 1;
    int gauche;
    int droite;

    printf("tmove : Debut de l'éxecution de periodique à 1s\n");
    rt_task_set_periodic(NULL, TM_NOW, 200e+6);

    while (1) {
        /* Attente de l'activation périodique */
        rt_task_wait_period(NULL);
        printf("tmove : Activation périodique\n");

        rt_mutex_acquire(&mutexEtat, TM_INFINITE);
        status = etatCommRobot;
        rt_mutex_release(&mutexEtat);

        //printf("tmove : status before = %d\n", status);
        
        if (status == STATUS_OK) {
            rt_mutex_acquire(&mutexMove, TM_INFINITE);
            switch (move->get_direction(move)) {
                case DIRECTION_FORWARD:
                    gauche = MOTEUR_ARRIERE_LENT;
                    droite = MOTEUR_ARRIERE_LENT;
                    break;
                case DIRECTION_LEFT:
                    gauche = MOTEUR_ARRIERE_LENT;
                    droite = MOTEUR_AVANT_LENT;
                    break;
                case DIRECTION_RIGHT:
                    gauche = MOTEUR_AVANT_LENT;
                    droite = MOTEUR_ARRIERE_LENT;
                    break;
                case DIRECTION_STOP:
                    gauche = MOTEUR_STOP;
                    droite = MOTEUR_STOP;
                    break;
                case DIRECTION_STRAIGHT:
                    gauche = MOTEUR_AVANT_LENT;
                    droite = MOTEUR_AVANT_LENT;
                    break;
            }
            rt_mutex_release(&mutexMove);

            status = robot->set_motors(robot, gauche, droite);
            check_status(status);
        }
    }
}

int write_in_queue(RT_QUEUE *msgQueue, void * data, int size) {
    // int rt_queue_write (RT_QUEUE ∗ q, const void ∗ buf, size_t size, int mode)
    void *msg;
    int err;

    msg = rt_queue_alloc(msgQueue, size);
    memcpy(msg, &data, size);

    if ((err = rt_queue_send(msgQueue, msg, sizeof (DMessage), Q_NORMAL)) < 0) {
        printf("Error msg queue send: %s\n", strerror(-err));
    }
    rt_queue_free(&queueMsgGUI, msg);

    return err;
}
