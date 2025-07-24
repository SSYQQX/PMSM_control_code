#ifndef _BSP_EPWM_H_
#define _BSP_EPWM_H_


#include "F28x_Project.h"

#define EPWM_EN_PIN 13//GPIO13
#define EPWM_ENABLE         0
#define EPWM_DISENABLE      1

#define EPWM_EN()           GPIO_WritePin(EPWM_EN_PIN, EPWM_ENABLE)
#define EPWM_DISEN()        GPIO_WritePin(EPWM_EN_PIN, EPWM_DISENABLE)


void bsp_epwm_init(void);
void bsp_epwm_EnPin_init(void);
extern Uint16 DEADTIME;
#endif /* _BSP_EPWM_H_ */


