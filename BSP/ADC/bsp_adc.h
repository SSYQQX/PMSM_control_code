

#ifndef BSP_ADC_H_
#define BSP_ADC_H_

#include "F28x_Project.h"

void bsp_adc_init(void);
void bsp_adc_start(void);

extern __interrupt void adca1_isr(void);
extern __interrupt void adcd1_isr(void);
extern void low_pass_filter_init(void);

typedef struct {
    float alpha;/* 滤波系数 */
    float f;/* 截止频率 */
    float t;/* 时间常数 */
}FILTE;

// 定义结构体，封装三相电流原始值、实际值与标幺值
typedef struct {
    // ADC 原始值
    uint16_t ia_adc;
    uint16_t ib_adc;
    uint16_t ic_adc;

    // 实际电流值（单位：A）
    float ia;
    float ib;
    float ic;

    // 标幺化后的电流值（单位：p.u.）
    float ia_pu;
    float ib_pu;
    float ic_pu;
} CurrentSampling_t;

// 各相电流的传感器标定系数
#define IA_GAIN  (0.0198f)
#define IA_OFFSET (-40.52f)

#define IB_GAIN  (0.0190f)
#define IB_OFFSET (-39.115f)

#define IC_GAIN  (0.0196f)
#define IC_OFFSET (-40.226f)

void CurrentSampling_Init(CurrentSampling_t *i_fb);
void CurrentSampling_ReadRaw(CurrentSampling_t *i_fb);
void CurrentSampling_Normalize(CurrentSampling_t *i_fb, float i_base);

extern CurrentSampling_t abc_current;
extern float base_current;

extern float M;

extern FILTE Vab_filte;
extern FILTE VBus_filte;
extern FILTE IBus_filte;
extern FILTE temp_filte;

/* 一阶滤波 */
#define PI              3.14159265f
#define TS              0.00005f/* 采样周期50us */

#endif /* BSP_ADC_H_ */

