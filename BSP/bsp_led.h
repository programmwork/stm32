/*******************************************************************************
    Filename: bsp_led.h

    Copyright 2019 Zhouq
***************************************************************************/
#ifndef BSP_LED_H
#define BSP_LED_H

#define LED_PORT_DIR   		P2DIR
#define LED_PORT_OUT		P2OUT
#define LED_PORT_IN			P2IN

#define LED_1				0X02
#define LED_2         		0X04
#define LED_ALL				0XFF

#include <stdint.h>

/*-------------------------------------------------------------
 *                  Function Prototypes
 * ------------------------------------------------------------*/
void bspLedInit( void );
void bspLedOn( uint8_t idx );
void bspLedOff( uint8_t idx );
void bspLedToggle( uint8_t idx );


#endif /* BSP_LED_H */
