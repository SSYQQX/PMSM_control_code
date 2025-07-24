/*
 * rampgen.h
 *
 *  Created on: 2025年7月18日
 *      Author: XUQQ
 */
#include <math.h>
#ifndef BSP_RAMPGEN_RAMPGEN_H_
#define BSP_RAMPGEN_RAMPGEN_H_

typedef struct {
    float Freq_Hz;          // 输入：目标频率 (Hz),通常为电频率
    float AngleStep_max;    // 参数：单位频率下的最大步进角度 (rad/step)，= 2π × Ts
    float Angle_rad;        // 变量：累积角度 (radian, 0~2π)
    float Out;              // 输出：当前角度，可用于查表 (sin/cos)
    float Ts;               // 参数：控制周期 (second)
    float Gain;             // 参数：增益系数（可用于调节幅值）
    float Offset;           // 参数：偏移量（用于叠加DC）
} RAMPGEN_FLOAT;

// 初始化宏
#define RAMPGEN_FLOAT_DEFAULTS(ts) \
{ \
    .Freq_Hz = 0.0f, \
    .AngleStep_max = 2.0f * 3.1415926f * (ts), \
    .Angle_rad = 0.0f, \
    .Out = 0.0f, \
    .Ts = (ts), \
    .Gain = 1.0f, \
    .Offset = 0.0f \
}


static inline void RAMPGEN_FLOAT_Run(RAMPGEN_FLOAT *v)
{
    // 每一步增加角度 = Freq × (2π × Ts)
    float angleStep = v->Freq_Hz * v->AngleStep_max;

    // 更新角度
    v->Angle_rad += angleStep;

    // 限制角度范围在 0 ~ 2π 之间，防止溢出
    if (v->Angle_rad > 2.0f * M_PI)
        v->Angle_rad -= 2.0f * M_PI;
    else if (v->Angle_rad < 0.0f)
        v->Angle_rad += 2.0f * M_PI;

    // 输出值可直接用于查表，或生成正弦/余弦参考值
    v->Out = v->Gain * v->Angle_rad + v->Offset;
}



#endif /* BSP_RAMPGEN_RAMPGEN_H_ */
