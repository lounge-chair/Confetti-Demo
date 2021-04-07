/*
 * Threads.h
 */

#ifndef THREADS_H_
#define THREADS_H_

#include <G8RTOS.h>

/* Semaphore/Resource Declarations */
extern semaphore_t LCDMutex;

/* Thread Declarations */
void readAccel(void);
void LCDtap(void);
void waitForTap(void);
void ballThread(void);
void idleThread(void);

typedef struct ball
{
    int16_t xPos;
    int16_t yPos;
    int16_t xVel;
    int16_t yVel;
    bool alive;
    char* threadID;
    uint16_t color;
} ball;

#endif /* THREADS_H_ */
