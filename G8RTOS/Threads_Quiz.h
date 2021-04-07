/*
 * Threads.h
 */

#ifndef THREADS_H_
#define THREADS_H_

#include <G8RTOS.h>

#define JOYSTICK_FIFO 0
#define TEMP_FIFO 1
#define LIGHT_FIFO 2

/* Semaphore/Resource Declarations */
extern semaphore_t LCDMutex;

/* Thread Declarations */
void startGame(void);
void readJoy(void);
void LCDtap(void);
void snakeThread(void);
void addSnake(void);
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
