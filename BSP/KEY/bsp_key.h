/*
 * bsp_key.h
 *
 *  Created on: 2024年12月2日
 *      Author: XUQQ
 */

#ifndef BSP_KEY_H_
#define BSP_KEY_H_

#include "F28x_Project.h"


extern int Turn_on_flag;//上电，下电标志

//按键中断初始化。Gpio初始化
void key_Init(void);
//按键中断
interrupt void xint1_isr(void);
interrupt void xint2_isr(void);


#endif /* BSP_KEY_H_ */
