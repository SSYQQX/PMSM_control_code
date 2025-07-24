/*
 * led.h
 *
 *  Created on: 2022��7��7��
 *      Author: Lenovo
 */

#ifndef BSP_LED_LED_H_
#define BSP_LED_LED_H_

#include "F28x_Project.h"

void GPIO_TogglePin(Uint16 gpioNumber);
void bsp_led_init(void);


#define LED1            89
#define LED2            90
#define LED3            24
#define LED4            25
#define LED5            26


#define LED2_ON()         GPIO_WritePin(LED2, 0)
#define LED3_ON()         GPIO_WritePin(LED3, 0)


#define RUN_LED_OFF()      GPIO_WritePin(RUN_LED, 0)
#define FLT_LED_OFF()      GPIO_WritePin(FLT_LED, 0)
#define DCBUS_LED_OFF()    GPIO_WritePin(DCBUS_LED, 0)

#define LED2_OFF()         GPIO_WritePin(LED2, 1)
#define LED3_OFF()         GPIO_WritePin(LED3, 1)

#define LED1_TOGGLE()      GPIO_TogglePin(LED1)
#define LED2_TOGGLE()      GPIO_TogglePin(LED2)
#define LED3_TOGGLE()      GPIO_TogglePin(LED3)
#define LED4_TOGGLE()      GPIO_TogglePin(LED4)
#define LED5_TOGGLE()      GPIO_TogglePin(LED5)


#endif /* BSP_LED_LED_H_ */
