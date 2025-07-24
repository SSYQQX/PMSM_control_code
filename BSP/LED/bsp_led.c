/*
 * led.c
 *
 *  Created on: 2022��7��7��
 *      Author: Lenovo
 */

#include "bsp_led.h"




void bsp_led_init(void)
{


    GPIO_SetupPinMux(LED1, GPIO_MUX_CPU1, 0x0);
    GPIO_SetupPinOptions(LED1, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(LED2, GPIO_MUX_CPU1, 0x0);
	GPIO_SetupPinOptions(LED2, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(LED3, GPIO_MUX_CPU1, 0x0);
	GPIO_SetupPinOptions(LED3, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(LED4, GPIO_MUX_CPU1, 0x0);
    GPIO_SetupPinOptions(LED4, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(LED5, GPIO_MUX_CPU1, 0x0);
    GPIO_SetupPinOptions(LED5, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_WritePin(LED1, 1);
    GPIO_WritePin(LED2, 1);
    GPIO_WritePin(LED3, 1);
    GPIO_WritePin(LED4, 1);
    GPIO_WritePin(LED5, 1);

}

void GPIO_TogglePin(Uint16 gpioNumber)
{
    static Uint16 outVal_led1;
    static Uint16 outVal_led2;
    static Uint16 outVal_led3;
    static Uint16 outVal_led4;
    static Uint16 outVal_led5;

    if(gpioNumber == LED1) {
        GPIO_WritePin(gpioNumber, outVal_led1);
        outVal_led1 = !outVal_led1;
    }
    if(gpioNumber == LED2) {
        GPIO_WritePin(gpioNumber, outVal_led2);
        outVal_led2 = !outVal_led2;
    }
    if(gpioNumber == LED3) {
        GPIO_WritePin(gpioNumber, outVal_led3);
        outVal_led3 = !outVal_led3;
    }
    if(gpioNumber == LED4) {
        GPIO_WritePin(gpioNumber, outVal_led4);
        outVal_led4 = !outVal_led4;
    }
    if(gpioNumber == LED5) {
        GPIO_WritePin(gpioNumber, outVal_led5);
        outVal_led5 = !outVal_led5;
    }
}

