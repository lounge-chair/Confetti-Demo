///////////////////////////////////////////////////////////////////////////////////
// Lab 4 - Main Method
// Description: Inits and launches an RTOS for moving balls demonstration
///////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <driverlib.h>
#include "msp.h"
#include "BSP.h"
#include "Threads.h"
#include "gpio.h"
#include <G8RTOS.h>

void main(void)
{
    /* Disable Watchdog */
    WDT_A_clearTimer();
    WDT_A_holdTimer();

    G8RTOS_Init();                          // initialize OS

    LCD_Init(true);                         // initialize LCD with touchpad

    G8RTOS_InitSemaphore(&LCDMutex, 1);

    G8RTOS_AddThread(&readAccel,    1,  "readAccel");             // add threads to scheduler
    G8RTOS_AddThread(&waitForTap,   1,  "waitForTap");
    G8RTOS_AddThread(&idleThread,   5,  "idleThread");
    G8RTOS_AddAPeriodicEvent(&LCDtap, 0,  PORT4_IRQn);

    G8RTOS_Launch();                        // launch OS

    while(1);
}
