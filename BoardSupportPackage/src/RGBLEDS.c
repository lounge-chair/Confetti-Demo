/*
 * RGBLEDS.c
 *
 *  Created on: Jan 21, 2021
 *      Author: Dan
 */

#include "RGBLEDS.h"

/*
 * LP3943_ColorSet
 * Sets the frequencies and PWM duty cycle
 * for each register of the specified unit
 */
//static void LP3943_ColorSet(uint32_t unit, uint32_t PWM_DATA);

/*
 * LP3943_LedModeSet
 * Sets each of the LEDs to the desired operating mode.
 * The modes are: ON, OFF, PWM1, PWM2
 */
void LP3943_LedModeSet(uint32_t unit, uint16_t LED_DATA)
{
    // Set initial peripheral address
    switch(unit) {
        case RED :
            UCB2I2CSA = 0x62;
            break;

        case GREEN :
            UCB2I2CSA = 0x61;
            break;

        case BLUE :
            UCB2I2CSA = 0x60;
            break;

        default :
            break;
    }

    uint8_t SHIFT_DATA[4] = {0};
    int start_pos = 0;

    for(int i = 0; i < 4; i++)
    {

        for(int j = start_pos; j < start_pos+4; j++)
        {
            if(LED_DATA & (1<<j))
            {
                SHIFT_DATA[i] |= 1 << (2*(j%4));
            }
        }
        start_pos += 4;
    }

    // Generate start condition
    UCB2CTLW0 |= UCTXSTT;

    // Wait for buffer availability
    while(UCB2CTLW0 & UCTXSTT);

    // Fill TXBUF with data for LP3943
    UCB2TXBUF = 0x16;

    // Wait for buffer availability
    while(!(UCB2IFG&UCTXIFG0));

    // Fill TXBUF with data for LP3943
    UCB2TXBUF = SHIFT_DATA[0];

    // Wait for buffer availability
    while(!(UCB2IFG&UCTXIFG0));

    UCB2TXBUF = SHIFT_DATA[1];

    // Wait for buffer availability
    while(!(UCB2IFG&UCTXIFG0));

    UCB2TXBUF = SHIFT_DATA[2];

    // Wait for buffer availability
    while(!(UCB2IFG&UCTXIFG0));

    UCB2TXBUF = SHIFT_DATA[3];

    // Wait for buffer availability
    while(!(UCB2IFG&UCTXIFG0));

    // Generate stop condition
    UCB2CTLW0 |= UCTXSTP;
}

/*
 * init_RGBLEDS
 * Performs initialization of RGB LEDs, and turns them off.
 */
void init_RGBLEDS()
{
    uint16_t UNIT_OFF = 0x0000;

    // Pulse SCL line to force minion device to release SDA
    P3DIR |=  GPIO_PIN7;
    P3DIR &= ~(GPIO_PIN7);

    // Software reset enable
    UCB2CTLW0 = UCSWRST;

    // Initialize I2C controller
    // Set as master, I2C mode, clock sync, SMCLK source, transmitter
    UCB2CTLW0 |= EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_MODE_3 | EUSCI_B_CTLW0_SYNC | EUSCI_B_CTLW0_SSEL__SMCLK | EUSCI_B_CTLW0_TR;

    // Set the FCLK as 400kHz
    // Presumes that the SMCLK is selected as source and fsmclk is 12MHz
    UCB2BRW = 30;

    // Sets pins as I2C mode
    // Table found on p160 of SLAS826E
    // Set P3.6 as UCB2_SDA and 3.7 as UCB2_SCL
    P3SEL0 |= P3_6_bm | P3_7_bm;
    P3SEL1 &= ~P3_6_bm & ~P3_7_bm;

    // Bitwise ANDing all of bits except UCSWRST
    UCB2CTLW0 &= ~UCSWRST;

    LP3943_LedModeSet(RED, UNIT_OFF);
    LP3943_LedModeSet(GREEN, UNIT_OFF);
    LP3943_LedModeSet(BLUE, UNIT_OFF);
}
