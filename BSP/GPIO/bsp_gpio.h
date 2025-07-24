/*
 * led.h
 *
 *  Created on: 2022��7��7��
 *      Author: Lenovo
 */

#ifndef BSP_GPIO_H_
#define BSP_GPIO_H_

#include "F28x_Project.h"

void bsp_gpio_init(void);


#define PWM1A            0
#define PWM1B            1
#define PWM2A            2
#define PWM2B            3
#define PWM3A            4
#define PWM3B            5


#define PWM1A_ON()         GPIO_WritePin(PWM1A, 1)
#define PWM1B_ON()         GPIO_WritePin(PWM1B, 1)
#define PWM2A_ON()         GPIO_WritePin(PWM2A, 1)
#define PWM2B_ON()         GPIO_WritePin(PWM2B, 1)
#define PWM3A_ON()         GPIO_WritePin(PWM3A, 1)
#define PWM3B_ON()         GPIO_WritePin(PWM3B, 1)


#define PWM1A_OFF()        GPIO_WritePin(PWM1A, 0)
#define PWM1B_OFF()        GPIO_WritePin(PWM1B, 0)
#define PWM2A_OFF()        GPIO_WritePin(PWM2A, 0)
#define PWM2B_OFF()        GPIO_WritePin(PWM2B, 0)
#define PWM3A_OFF()        GPIO_WritePin(PWM3A, 0)
#define PWM3B_OFF()        GPIO_WritePin(PWM3B, 0)

#endif /* BSP_GPIO_H_ */
