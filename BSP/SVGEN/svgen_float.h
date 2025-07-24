/*
 * svgen_float.h
 *
 *  Created on: 2025年7月18日
 *      Author: XUQQ
 */

#ifndef BSP_SVGEN_SVGEN_FLOAT_H_
#define BSP_SVGEN_SVGEN_FLOAT_H_

#include <math.h>
#include <stdint.h>

// SVPWM 数据结构（float32 版本）
typedef struct {
    float Ualpha;      // 输入：α轴参考电压 (p.u.)
    float Ubeta;       // 输入：β轴参考电压 (p.u.)
    float Ta;          // 输出：A相占空比 (0~1)
    float Tb;          // 输出：B相占空比
    float Tc;          // 输出：C相占空比
    float tmp1;        // 中间变量：Ubeta
    float tmp2;        // 中间变量：Ubeta/2 + √3/2 * Ualpha
    float tmp3;        // 中间变量：tmp2 - tmp1
    uint16_t VecSector; // 空间矢量扇区编号 (1~6)
} SVGEN_FLOAT;

// 默认初始化宏
#define SVGEN_FLOAT_DEFAULTS { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 }

// SVPWM 主调制函数（推荐替代宏）
static inline void SVGEN_FLOAT_run(SVGEN_FLOAT *v)
{
    // 计算中间变量
    v->tmp1 = v->Ubeta;
    v->tmp2 = 0.5f * v->Ubeta + 0.8660254f * v->Ualpha;  // √3/2 ≈ 0.8660254
    v->tmp3 = v->tmp2 - v->tmp1;

    // 扇区判断逻辑（无三角函数）
    v->VecSector = 3;
    if (v->tmp2 > 0) v->VecSector -= 1;
    if (v->tmp3 > 0) v->VecSector -= 1;
    if (v->tmp1 < 0) v->VecSector = 7 - v->VecSector;

    // 计算三相调制占空比
    if (v->VecSector == 1 || v->VecSector == 4) {
        v->Ta = v->tmp2;
        v->Tb = v->tmp1 - v->tmp3;
        v->Tc = -v->tmp2;
    } else if (v->VecSector == 2 || v->VecSector == 5) {
        v->Ta = v->tmp3 + v->tmp2;
        v->Tb = v->tmp1;
        v->Tc = -v->tmp1;
    } else {
        v->Ta = v->tmp3;
        v->Tb = -v->tmp3;
        v->Tc = -(v->tmp1 + v->tmp2);
    }

    // 可选：裁剪输出范围在[0, 1]之间（用于PWM比较寄存器）
     v->Ta = fminf(fmaxf(v->Ta, -1.0f), 1.0f);
     v->Tb = fminf(fmaxf(v->Tb, -1.0f), 1.0f);
     v->Tc = fminf(fmaxf(v->Tc, -1.0f), 1.0f);
}



#endif /* BSP_SVGEN_SVGEN_FLOAT_H_ */
