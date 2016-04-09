#include "video.h"


void setImage(DImage* img)
{
   rt_mutex_acquire(&mutexImage, TM_INFINITE);
   memcpy(image, img, sizeof(DImage)); 
   rt_mutex_release(&mutexImage);
}

int getMonitorStatus()
{
    int comStatus;
    rt_mutex_acquire(&mutexEtat, TM_INFINITE);
    comStatus = etatCommMoniteur;
    rt_mutex_release(&mutexEtat);
    return comStatus;
}


void computeArena(void* arg)
{

}

void computePosition(void* arg)
{

}

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

        // Vérification de l'état de comm avec le moniteur
        comStatus = getMonitorStatus();
        if(comStatus != STATUS_OK)
            continue;
        
        // Obtention de l'image
        camera->get_frame(camera, img);
        setImage(img); 

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
               message->free(message);
            }
        }
    }

    img->free(img);
}
