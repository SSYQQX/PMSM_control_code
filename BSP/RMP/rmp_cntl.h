/*
 * rmp_cntl.h
 *
 *  Created on: 2025年7月18日
 *      Author: XUQQ
 */

#ifndef BSP_RMP_RMP_CNTL_H_
#define BSP_RMP_RMP_CNTL_H_

typedef struct {
    float  TargetValue;     // 目标值
    float  SetpointValue;   // 当前输出值
    float  RampHighLimit;   // 上限
    float  RampLowLimit;    // 下限
    float  StepSize;        // 每次步进值
    uint32_t RampDelayMax;  // 计数器最大值（多少次循环后才允许改变）
    uint32_t RampDelayCount;// 当前计数
    uint32_t EqualFlag;     // 达到目标值的标志位（1=已达到）
} RMPCNTL_FLOAT;

#define RMPCNTL_FLOAT_DEFAULTS  {  \
    0.0f, /* TargetValue */        \
    0.0f, /* SetpointValue */      \
    3000.0f, /* RampHighLimit */      \
   -3000.0f, /* RampLowLimit  */      \
    0.00001f, /* StepSize */         \
    1,    /* RampDelayMax */       \
    0,    /* RampDelayCount */     \
    0     /* EqualFlag */          \
}

// 函数接口
static inline void RC_FLOAT_Run(RMPCNTL_FLOAT *v)
{
    float diff = v->TargetValue - v->SetpointValue;

    if (fabsf(diff) >= v->StepSize) {
        v->RampDelayCount++;
        v->EqualFlag = 0;
        if (v->RampDelayCount >= v->RampDelayMax) {
            if (diff > 0.0f)
                v->SetpointValue += v->StepSize;
            else
                v->SetpointValue -= v->StepSize;

            if (v->SetpointValue > v->RampHighLimit)
                v->SetpointValue = v->RampHighLimit;
            else if (v->SetpointValue < v->RampLowLimit)
                v->SetpointValue = v->RampLowLimit;

            v->RampDelayCount = 0;
        }
    } else {
        v->EqualFlag = 1;
    }
}



#endif /* BSP_RMP_RMP_CNTL_H_ */
