
#ifndef VIDEO_H
#define	VIDEO_H

#include "global.h"
#include "includes.h"

#ifdef	__cplusplus
extern "C" {
#endif
        void computeArena(void *arg);
        void computePosition(void* arg);
        void acquireImage(void* arg);
#ifdef	__cplusplus
}
#endif

#endif	/* VIDEO_H */

