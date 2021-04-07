#include <BSP.h>
#include <G8RTOS_IPC.h>
#include "msp.h"
#include "Threads_Quiz.h"
#include <stdbool.h>
#include <stdlib.h>

#define BUFF_SIZE 256
semaphore_t LCDMutex;
volatile bool lcd_flag = false;
volatile int16_t joyX = 0, joyY = 0;

typedef enum joyDir{up, down, left, right} joyDir;
joyDir currDir = right, lastDir = right;

typedef struct snake{
   int16_t x;
   int16_t y;
   bool alive;
} snake;

typedef struct cb
{
    snake buffer[BUFF_SIZE];
    snake * head;
    snake * tail;
    uint32_t lostData;
    semaphore_t currentSize;
    semaphore_t mutex;
} cb;

cb cbuff;

void startGame(void)
{
    while(1) {
        // kill all other threads on arrival
        G8RTOS_KillAllOthers();
        // display waiting prompt
        LCD_Text(MAX_SCREEN_X/4, MAX_SCREEN_Y/2, "Tap for game start!", LCD_WHITE);
        // wait for lcd_flag
        while(!lcd_flag);
        // disable touch interrupt and clear flag
        P4->IFG &= ~BIT0;
        NVIC_DisableIRQ(PORT4_IRQn);
        P4->IE &= ~BIT0;
        lcd_flag = false;
        // clear screen
        LCD_Clear(LCD_BLACK);
        // init circular buffer
        cbuff.head = &cbuff.buffer[0];
        cbuff.tail = &cbuff.buffer[0];
        cbuff.lostData = 0;
        G8RTOS_InitSemaphore(&cbuff.currentSize, 0); // init to 0 because empty FIFOs should block threads until it has data
        G8RTOS_InitSemaphore(&cbuff.mutex, 1);       // init to 1 because no thread currently is reading from this FIFO
        cbuff.buffer[0].x = MAX_SCREEN_X/2;
        cbuff.buffer[0].y = MAX_SCREEN_Y/2;
        cbuff.buffer[0].alive = true;
        // add snakeThread, joyThread and idle thread
        G8RTOS_AddThread(&snakeThread,    0,  "snakeThread");
        G8RTOS_AddThread(&addSnake,    0,  "addSnake");
        G8RTOS_AddThread(&readJoy,    0,  "readJoy");
        G8RTOS_AddThread(&idleThread,   5,  "idleThread");
        // kill self
        G8RTOS_KillSelf();
    }
}

void snakeThread(void)
{
    while(1)
    {
        for(int i = 0; i < BUFF_SIZE; i++)
        {
            if(cbuff.buffer[i].alive)
            {
                LCD_DrawRectangle(cbuff.buffer[i].x, cbuff.buffer[i].x+4, cbuff.buffer[i].y, cbuff.buffer[i].y+4, LCD_WHITE);
            }
        }
    }
}

void addSnake(void)
{
    snake tempSnake;
    snake lastSnake = cbuff.buffer[0];
    while(1)
    {
        if(currDir == up && lastDir != down) {
                tempSnake.x = lastSnake.x;
                tempSnake.y = lastSnake.y-2;
                tempSnake.alive = true;
            }
            else if(currDir == left && lastDir != right) {
                tempSnake.x = lastSnake.x-2;
                tempSnake.y = lastSnake.y;
                tempSnake.alive = true;
            }
            else if(currDir == right && lastDir != left) {
                tempSnake.x = lastSnake.x+2;
                tempSnake.y = lastSnake.y;
                tempSnake.alive = true;
            }
            else if(currDir == down && lastDir != up) {
                tempSnake.x = lastSnake.x;
                tempSnake.y = lastSnake.y+2;
                tempSnake.alive = true;
            }

            if (cbuff.currentSize > BUFF_SIZE - 1)       // check if current size is at full capacity
                {
                cbuff.currentSize = BUFF_SIZE - 1;       // discard new data, increment lostData, return error
                cbuff.lostData++;
                    return 1;
                }
                else
                {
                    *cbuff.tail = tempSnake;
                    G8RTOS_ReleaseSemaphore(&cbuff.currentSize);     // release currentSize semaphore
                    cbuff.tail++;                                        // increment tail
                    if(cbuff.tail == (int32_t*)&cbuff.head)   // if tail has gone out of bounds...
                    {
                        cbuff.tail = cbuff.buffer;            // reset tail to start of buffer
                    }
                }
            lastSnake = tempSnake;
            G8RTOS_Sleep(30);
    }
}

//void delSnake(void)
//{
//    snake deadSnake;
//    deadSnake.x = 0;
//    deadSnake.y = 0;
//    deadSnake.alive = false;
//    while(1)
//    {
//        *cbuff.head = deadSnake;
//
//    }
//}

void readJoy(void)
{
    while(1)
    {
        lastDir = currDir;
        GetJoystickCoordinates(&joyX, &joyY);
        if((abs(joyX) > 550) || (abs(joyY) > 550))
        {
            if(abs(joyX) > abs(joyY))
            {
                if(joyX > 0)    currDir = left;
                else            currDir = right;
            }
            else
            {
                if(joyY > 0)    currDir = down;
                else            currDir = up;
            }
        }
        G8RTOS_Sleep(50);
    }
}

void LCDtap(void)
{
    Delay(1);
    if(P4->IFG & BIT0)
    {
        P4->IFG &= ~BIT0;
        NVIC_DisableIRQ(PORT4_IRQn);
        P4->IE &= ~BIT0;
        lcd_flag = true;
    }
}

/*
 * Idle Thread
 */
void idleThread(void)
{
    while(1);
}
