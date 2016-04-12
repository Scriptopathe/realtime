#include "video.h"



/**
 * Tâche de calcul de l'arène.
 */
void computeArena(void* arg)
{
    DJpegimage* jpeg = d_new_jpegimage();
    DImage* img = d_new_image();
    int comStatus;

    while(1)
    {
        // On attend l'ordre de trouver l'arène.
        rt_sem_p(&semArena, TM_INFINITE);
        setFindingArena(1);
        rt_printf("\033[31;1mARENA GP \033[0m");

        if(image == NULL)
            continue;

        rt_mutex_acquire(&mutexArena, TM_INFINITE);
        rt_mutex_acquire(&mutexImage, TM_INFINITE);

        tmpArena = image->compute_arena_position(image);

        rt_mutex_acquire(&mutexCamera, TM_INFINITE);
        camera->get_frame(camera, img);
        rt_mutex_release(&mutexCamera);
        setImage(img);
        

        if(tmpArena != NULL)
        {
            rt_printf("ARENA TROUVEE !!!!!!! YEESSS !\n");
            d_imageshop_draw_arena(image, tmpArena);
        }
        else
        {
            rt_printf("ARENA NULL !!!!!!! !\n");
        }

        // Compression

        jpeg->compress(jpeg, image);

        // Envoi au moniteur
        comStatus = getMonitorStatus();
        if(comStatus == STATUS_OK)
        {
            DMessage* message = d_new_message();
            message->put_jpeg_image(message, jpeg);
            if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0)

            {
               //message->free(message);
            }
        }

        rt_mutex_release(&mutexImage);
        rt_mutex_release(&mutexArena);
    }

}

void computePosition(void* arg)
{
 
}

/**
 * Tâche d'acquisition de l'image.
 */
void acquireImage(void* arg)
{
    int comStatus = STATUS_OK;
    DImage* img = d_new_image();
    DJpegimage *jpeg = d_new_jpegimage();

    // Chaque 600ms
    rt_task_set_periodic(NULL, TM_NOW, 600e+6);

    // Ouvre la connexion avec la caméra.
    camera->open(camera);

    while (1) {
        /* Attente de l'activation périodique */
        rt_task_wait_period(NULL);
        rt_printf("tImageAcquisition : Activation périodique\n");

        // Si on effectue la recherche d'arène : on ne commence pas
        // l'acquisition de nouvelles images
        if(isFindingArena())
            continue;

        // Vérification de l'état de comm avec le moniteur
        comStatus = getMonitorStatus();
        if(comStatus != STATUS_OK)
            continue;
        
        
        // Obtention de l'image
        rt_mutex_acquire(&mutexCamera, TM_INFINITE);
        camera->get_frame(camera, img);
        rt_mutex_release(&mutexCamera);

        setImage(img); 

        // Traçage de la position (si activé)
        if(posComputeEnabled())
        {
            rt_printf("\033[1;32mPOSITION COMPUTE ENABLED. \033[0m\n");
            if(arena != NULL)
            {
                DPosition* pos = img->compute_robot_position(img, arena);
                if(pos != NULL)
                {
                    rt_printf("\033[1;32mPOSITION NOT NULL. \033[0m\n");
                    d_imageshop_draw_position(img, pos);
                    
                    // Message avec la position
                    DMessage* message = d_new_message();
                    message->put_position(message, pos);
                    if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0)
                    {
                        // message->free(message);
                    }
                }
            }
        }

        rt_mutex_acquire(&mutexArena, TM_INFINITE);
        if(arena != NULL)
        {
            d_imageshop_draw_arena(img, arena);
        }
        rt_mutex_release(&mutexArena);
        
        // Compression
        jpeg->compress(jpeg, img); 

        // Envoi au moniteur
        comStatus = getMonitorStatus();
        if(comStatus == STATUS_OK)
        {
            DMessage* message = d_new_message();
            message->put_jpeg_image(message, jpeg);
            if (write_in_queue(&queueMsgGUI, message, sizeof (DMessage)) < 0)
            {
               //message->free(message);
            }
        }
    }

    img->free(img);
}
