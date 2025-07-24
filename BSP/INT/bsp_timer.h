#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_


#include "F28x_Project.h"


void bsp_timer0_init(void);
__interrupt void cpu_timer0_isr(void);

void bsp_timer1_init(void);
__interrupt void cpu_timer1_isr(void);

void bsp_timer2_init(void);
__interrupt void cpu_timer2_isr(void);

#endif /* _BSP_TIMER_H_ */


