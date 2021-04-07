#include <BSP.h>
#include <G8RTOS_IPC.h>
#include "msp.h"
#include "Threads.h"
#include <stdbool.h>
#include <stdlib.h>

#define MAX_BALLS   20
#define EDGE_MASK   5
semaphore_t sensorMutex;
semaphore_t LCDMutex;
volatile bool lcd_flag = false;
volatile int16_t accelX = 0, accelY = 0;
volatile Point tp_data;
volatile uint16_t NumberOfBalls = 0;

ball ballArray[MAX_BALLS] = {0,0,0,0,false,"",LCD_WHITE};

void readAccel(void)
{
    while(1) {
        //  Read from the accelerometer’s x- and y- axis and save the value into global vars
            while(bmi160_read_accel_x(&accelX));
            while(bmi160_read_accel_y(&accelY));
        //  Sleep
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

void waitForTap(void) {
    bool addBall = true;
    while(1)
    {
        if(lcd_flag == true)
        {
            addBall = true;
            // Read touch coordinates
            G8RTOS_AcquireSemaphore(&LCDMutex);
            tp_data = TP_ReadXY();
            G8RTOS_ReleaseSemaphore(&LCDMutex);
            // Check for out of bounds touch
            if(tp_data.x < MIN_SCREEN_X+EDGE_MASK || tp_data.x > MAX_SCREEN_X-EDGE_MASK || tp_data.y < MIN_SCREEN_Y+EDGE_MASK || tp_data.y > MAX_SCREEN_Y-EDGE_MASK)
            {
                addBall = false;
            }
            else
            {
                // Iterate through all balls
                // Determine whether to add or delete ball
                for(int i = 0; i < MAX_BALLS; i++)
                {
                    if(ballArray[i].alive)
                    {
                        if((tp_data.x >= ballArray[i].xPos-30 && tp_data.x <= ballArray[i].xPos+34) && (tp_data.y >= ballArray[i].yPos-30 && tp_data.y <= ballArray[i].yPos+34))
                        {
                            ballArray[i].alive = false;
                            addBall = false;
                            break;
                        }
                    }
                }
                if(addBall && (NumberOfBalls < MAX_BALLS)) {
                    G8RTOS_AddThread(&ballThread,   1,  "ballThread");
                }
            }
            // Reset LCD flag
            lcd_flag = false;
            G8RTOS_Sleep(500);
            //lcd_flag = false;
            P4->IE |= BIT0;
            P4->IFG &= ~BIT0;
            NVIC_EnableIRQ(PORT4_IRQn);
        }
    }
}
void ballThread(void)
{
    NumberOfBalls++;
    int index = -99;
    uint32_t color = 0xFFFF;
    int randColor = SystemTime % 12;
    int randSpeed = SystemTime % 5 + 1;
    switch(randColor)
    {
    case 0:
        color = LCD_WHITE;
        break;
    case 1:
        color = LCD_BLUE;
        break;
    case 2:
        color = LCD_RED;
        break;
    case 3:
        color = LCD_MAGENTA;
        break;
    case 4:
        color = LCD_GREEN;
        break;
    case 5:
        color = LCD_CYAN;
        break;
    case 6:
        color = LCD_YELLOW;
        break;
    case 7:
        color = LCD_GRAY;
        break;
    case 8:
        color = LCD_PURPLE;
        break;
    case 9:
        color = LCD_ORANGE;
        break;
    case 10:
        color = LCD_PINK;
        break;
    case 11:
        color = LCD_OLIVE;
        break;
    }
    // find first dead ball in ball array
    for(int i = 0; i < MAX_BALLS; i++)
    {
        if(!ballArray[i].alive)
        {
            index = i;
            break;
        }
    }
    // make it alive
    ballArray[index].alive = true;

    // initialize position
    ballArray[index].xPos = tp_data.x;
    ballArray[index].yPos = tp_data.y;

    while(1) {
        // acquire lcd mutex
        G8RTOS_AcquireSemaphore(&LCDMutex);
        // erase old ball position
        LCD_DrawRectangle(ballArray[index].xPos, ballArray[index].xPos+4, ballArray[index].yPos, ballArray[index].yPos+4, LCD_BLACK);
        // release lcd mutex
        G8RTOS_ReleaseSemaphore(&LCDMutex);

        // check if ball is alive, if not kill self
        if(!ballArray[index].alive)
        {
            NumberOfBalls--;
            G8RTOS_KillSelf();
        }

        // update ball position based on accel
        if(accelX > 6000)       ballArray[index].xVel = 4;
        else if(accelX > 4000)  ballArray[index].xVel = 3;
        else if(accelX > 2000)  ballArray[index].xVel = 2;
        else if(accelX > 500)   ballArray[index].xVel = 1;
        else if(accelX < -6000) ballArray[index].xVel = -4;
        else if(accelX < -4000) ballArray[index].xVel = -3;
        else if(accelX < -2000) ballArray[index].xVel = -2;
        else if(accelX < -500)  ballArray[index].xVel = -1;
        else                    ballArray[index].xVel = 0;

        ballArray[index].xVel *= randSpeed;

        if(accelY > 6000)       ballArray[index].yVel = -4;
        else if(accelY > 4000)  ballArray[index].yVel = -3;
        else if(accelY > 2000)  ballArray[index].yVel = -2;
        else if(accelY > 500)   ballArray[index].yVel = -1;
        else if(accelY < -6000) ballArray[index].yVel = 4;
        else if(accelY < -4000) ballArray[index].yVel = 3;
        else if(accelY < -2000) ballArray[index].yVel = 2;
        else if(accelY < -500)  ballArray[index].yVel = 1;
        else                    ballArray[index].yVel = 0;

        ballArray[index].yVel *= randSpeed;

        // X coord wrap check
        if(ballArray[index].xPos + ballArray[index].xVel > MAX_SCREEN_X)
        {
           ballArray[index].xPos = 0;
        }
        else if(ballArray[index].xPos + ballArray[index].xVel < MIN_SCREEN_X)
        {
           ballArray[index].xPos = MAX_SCREEN_X - 4;
        }
        else
        {
           ballArray[index].xPos = ballArray[index].xPos + ballArray[index].xVel;
        }
        // Y coord wrap check
        if(ballArray[index].yPos + ballArray[index].yVel > MAX_SCREEN_Y)
        {
           ballArray[index].yPos = 0;
        }
        else if(ballArray[index].yPos + ballArray[index].yVel < MIN_SCREEN_Y)
        {
           ballArray[index].yPos = MAX_SCREEN_Y - 4;
        }
        else
        {
           ballArray[index].yPos = ballArray[index].yPos + ballArray[index].yVel;
        }

        // acquire lcd mutex
        G8RTOS_AcquireSemaphore(&LCDMutex);
        // draw new ball position
        LCD_DrawRectangle(ballArray[index].xPos, ballArray[index].xPos+4, ballArray[index].yPos, ballArray[index].yPos+4, color);
        // release lcd mutex
        G8RTOS_ReleaseSemaphore(&LCDMutex);
        // sleep 30 ms
        G8RTOS_Sleep(30);
    }
}

/*
 * Idle Thread
 */
void idleThread(void)
{
    while(1);
}
