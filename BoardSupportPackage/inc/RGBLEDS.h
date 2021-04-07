/*
 * RGBLEDS.h
 *
 *  Created on: Jan 21, 2021
 *      Author: Dan
 */

#ifndef RGBLEDS_H_
#define RGBLEDS_H_

#include <driverlib.h>
#include "msp.h"

#define P3_6_bm  0x40
#define P3_7_bm  0x80

typedef enum device
{
    BLUE = 0,
    GREEN = 1,
    RED = 2
} unit_desig;


/*
 * LP3943_ColorSet
 * Sets the frequencies and PWM duty cycle
 * for each register of the specified unit
 */
static void LP3943_ColorSet(uint32_t unit, uint32_t PWM_DATA);

/*
 * LP3943_LedModeSet
 * Sets each of the LEDs to the desired operating mode.
 * The modes are: ON, OFF, PWM1, PWM2
 */
void LP3943_LedModeSet(uint32_t unit, uint16_t LED_DATA);

/*
 * init_RGBLEDS
 * Performs initialization of RGB LEDs, and turns them off.
 */
void init_RGBLEDS();

#endif /* RGBLEDS_H_ */
