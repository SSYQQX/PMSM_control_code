/*
 * led.c
 *
 *  Created on: 2022��7��7��
 *      Author: Lenovo
 */

#include "bsp_gpio.h"

void bsp_gpio_init(void)
{


    GPIO_SetupPinMux(PWM1A, GPIO_MUX_CPU1, 0x0);
    GPIO_SetupPinOptions(PWM1A, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(PWM1B, GPIO_MUX_CPU1, 0x0);
	GPIO_SetupPinOptions(PWM1B, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(PWM2A, GPIO_MUX_CPU1, 0x0);
	GPIO_SetupPinOptions(PWM2A, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(PWM2B, GPIO_MUX_CPU1, 0x0);
    GPIO_SetupPinOptions(PWM2B, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(PWM3A, GPIO_MUX_CPU1, 0x0);
    GPIO_SetupPinOptions(PWM3A, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(PWM3B, GPIO_MUX_CPU1, 0x0);
    GPIO_SetupPinOptions(PWM3B, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_WritePin(PWM1A, 0);
    GPIO_WritePin(PWM1B, 0);
    GPIO_WritePin(PWM2A, 0);
    GPIO_WritePin(PWM2B, 0);
    GPIO_WritePin(PWM3A, 0);
    GPIO_WritePin(PWM3B, 0);
}


