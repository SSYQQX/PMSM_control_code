#include "device.h"

#include "F28x_Project.h"

#include "interrupt.h"
#include "bsp_emif.h"
#include "bsp_led.h"
#include "bsp_timer.h"
#include "sysctl.h"
#include "emif.h"
#include "bsp_adc.h"
#include "bsp_pid_ctrl.h"
#include "bsp_epwm.h"
#include "bsp_adc.h"
#include "bsp_eQEP.h"
#include "bsp_timer.h"
#include "bsp_gpio.h"
#include "bsp_key.h"
#include "bsp_can.h"


#include "driverlib.h"

#include "Solar_F.h"

#include "rmp_cntl.h"
#include "rampgen.h"
#include "svgen_float.h"
/**
 * main.c
 */
__interrupt void adca1_isr(void);

uint16_t start_flag=0;
uint16_t stop_flag=0;

int Turn_on_flag=0;

int16 SpeedRef=100;//rpm
float Vq_test=0;
float VdTesting=0;
float VqTesting=0.5;
float theta = 0;
float sin_theta = 0;
float cos_theta = 0;

float angle[400]={0};
float ta[400]={0};
float tb[400]={0};
float tc[400]={0};
int sample_index=0;

float ialpha[400]={0};
float ibeta[400]={0};

float id[400]={0};
float iq[400]={0};
RMPCNTL_FLOAT rmp = RMPCNTL_FLOAT_DEFAULTS;
//参考角度
RAMPGEN_FLOAT rampGen = RAMPGEN_FLOAT_DEFAULTS(0.000050f); // 控制周期50μs

iPARK_F iPark_voltage;
PARK_F  Park_current;
CLARKE_F Clarke_current;
SVGEN_FLOAT svgen = SVGEN_FLOAT_DEFAULTS;//定义并初始化

int i=0;
int can_send_step=0;
int count=0;

int main(void)
{
    //////////////// 初始化系统////////////////
	InitSysCtrl();
	InitGpio();
	DINT;
	InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    //////////////// 初始化外设和应用层模块////////////////
    ///////PWM///////
    //PWM使能引脚初始化
    bsp_epwm_EnPin_init();
    //PWM初始化
    bsp_epwm_init();
    //PWM缓冲芯片使能
    EPWM_EN();

    ///////ADC///////
    bsp_adc_init();
    //ADC电流采样变量初始化
    CurrentSampling_Init(&abc_current);
    //一阶低通滤波器
    low_pass_filter_init();

    ///////编码器///////
     Init_Variables();
     Init_EQEP1_Gpio();
     Init_EQEP1();

    ///////定时器配置///////
     //   初始化CPU定时器0/1/2
     InitCpuTimers();//只用初始化一次
     bsp_timer0_init();
     bsp_timer1_init();
     bsp_timer2_init();

     ///////  CAN  ///////
     bsp_can_init(); // 初始化CAN模块

    //状态指示灯
	bsp_led_init();
	//按键
	key_Init();

	//开启全局中断
    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM

    //////////////// 初始化控制模块 ////////////////

    ///////斜坡控制函数///////
    rmp.TargetValue = SpeedRef;
    rmp.StepSize = 0.05f;

    ///////坐标变换函数///////
    //初始化park反变换
    iPARK_F_init(&iPark_voltage);
    //初始化park变换
    PARK_F_init(&Park_current);
    //初始化clarke变换
    CLARKE_F_init(&Clarke_current);

    //状态指示
    //LED2_ON();

	for(;;)
	{
        DEVICE_DELAY_US(500000);
	    if(start_flag==1)
	    {
	        LED2_TOGGLE();
	    }
	}
}


__interrupt void adca1_isr(void)
{

    /////////////////////////////采样///////////////////////////////////////////
    CurrentSampling_ReadRaw(&abc_current);
    CurrentSampling_Normalize(&abc_current,base_current);

    //斜坡函数,产生转速参考
    rmp.TargetValue = (float)SpeedRef;
    RC_FLOAT_Run(&rmp);

    //参考角度
    rampGen.Freq_Hz = rmp.SetpointValue; // 设定频率，电频率,与转速相关
    RAMPGEN_FLOAT_Run(&rampGen);
    // 得到当前角度：
    theta = rampGen.Angle_rad;
    sin_theta = sinf(theta);
    cos_theta = cosf(theta);



    //反park变换
    iPark_voltage.d=VdTesting;
    iPark_voltage.q=VqTesting;
    iPark_voltage.cos=cos_theta;
    iPark_voltage.sin=sin_theta;
    iPARK_F_FUNC(&iPark_voltage);

    // 输入来自逆Park变换的 αβ 电压
    svgen.Ualpha = iPark_voltage.alpha;
    svgen.Ubeta  = iPark_voltage.beta;

    // 执行SVPWM计算
    SVGEN_FLOAT_run(&svgen);

    abc_current.ia_pu=0.5*cosf(theta);
    abc_current.ib_pu=0.5*cosf(theta-2*M_PI/3);
    abc_current.ic_pu=0.5*cosf(theta-4*M_PI/3);

    Clarke_current.a=abc_current.ia_pu;
    Clarke_current.b=abc_current.ib_pu;
    Clarke_current.c=abc_current.ic_pu;
    CLARKE_F_FUNC(&Clarke_current);

    Park_current.alpha=Clarke_current.alpha;
    Park_current.beta=Clarke_current.beta;
    Park_current.cos=cos_theta;
    Park_current.sin=sin_theta;
    PARK_F_FUNC(&Park_current);

    switch (can_send_step)
    {
        case 0:
            bsp_can_send_float(theta, 0x22, CANA_TX_MSG_OBJ_THETA);
            break;
        case 1:
            break;
        case 2:
            bsp_can_send_two_floats(svgen.Ta, svgen.Tb, 0x33, CANA_TX_MSG_OBJ_SVGEN);
            break;
        case 3:
            break;
        default:
            //can_send_step = 0;
            return;
    }

    // 步进控制：0 → 1 → 0 → ...
    can_send_step++;
    if (can_send_step > 3)
        can_send_step = 0;
//    bsp_can_send_float(theta,  0x22,CANA_TX_MSG_OBJ_THETA);
//    bsp_can_send_two_floats(svgen.Ta, svgen.Tb,0x33,CANA_TX_MSG_OBJ_SVGEN);

    //变量记录观测
    if(sample_index<400)
    {
    //三相调制
//    ta[sample_index]=svgen.Ta;
//    tb[sample_index]=svgen.Tb;
//    tc[sample_index]=svgen.Tc;
//    angle[sample_index]=theta;

//    ialpha[sample_index]=Clarke_current.alpha;
//    ibeta[sample_index]=Clarke_current.beta;
//
//    id[sample_index]=Park_current.d;
//    iq[sample_index]=Park_current.q;

    sample_index++;
    }
    else
    {
        sample_index=0;
    }

//    // ------------------------------------------------------------------------------
//        EPwm1Regs.CMPA.bit.CMPA = (INV_PWM_HALF_TBPRD*svgen1.Ta)+INV_PWM_HALF_TBPRD;
//        EPwm2Regs.CMPA.bit.CMPA = (INV_PWM_HALF_TBPRD*svgen1.Tb)+INV_PWM_HALF_TBPRD;
//        EPwm3Regs.CMPA.bit.CMPA = (INV_PWM_HALF_TBPRD*svgen1.Tc)+INV_PWM_HALF_TBPRD;
////    ////////////////////加载比较值////////////////////////
    EPwm1Regs.CMPA.bit.CMPA =  EPwm1Regs.TBPRD*((1.0+M*svgen.Ta)/2.0);
    EPwm1Regs.CMPB.bit.CMPB =  EPwm1Regs.TBPRD*((1.0+M*svgen.Ta)/2.0);
    EPwm2Regs.CMPA.bit.CMPA =  EPwm2Regs.TBPRD*((1.0+M*svgen.Tb)/2.0);
    EPwm2Regs.CMPB.bit.CMPB =  EPwm2Regs.TBPRD*((1.0+M*svgen.Tb)/2.0);
    EPwm3Regs.CMPA.bit.CMPA =  EPwm3Regs.TBPRD*((1.0+M*svgen.Tc)/2.0);
    EPwm3Regs.CMPB.bit.CMPB =  EPwm3Regs.TBPRD*((1.0+M*svgen.Tc)/2.0);

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void cpu_timer0_isr(void)
{
    //CpuTimer0Regs.TCR.bit.TIF = 1; // 清除中断标志
    LED3_TOGGLE();
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void cpu_timer1_isr(void)
{
    //CpuTimer1Regs.TCR.bit.TIF = 1; // 清除中断标志
    LED4_TOGGLE();
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void cpu_timer2_isr(void)
{
    //CpuTimer1Regs.TCR.bit.TIF = 1; // 清除中断标志
    //LED1_TOGGLE();
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void CANA_ISR(void)
{
    count++;
    cana_isr_status = CAN_getInterruptCause(CANA_BASE);

    if(cana_isr_status == CAN_INT_INT0ID_STATUS)
    {
        // 读取 CAN 状态，判断是否有错误
        cana_isr_status = CAN_getStatus(CANA_BASE);
        if(((cana_isr_status & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 7) &&
           ((cana_isr_status & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 0))
        {
            cana_error_flag = 1;
        }
    }
    else if(cana_isr_status == CANA_TX_MSG_OBJ_ID)
    {
        // 发送成功
        CAN_clearInterruptStatus(CANA_BASE, CANA_TX_MSG_OBJ_ID);
        cana_tx_count++;
        cana_error_flag = 0;
    }
    else if (cana_isr_status >= 10 && cana_isr_status <= 32)
    {
        // 接收数据
        bool OK=CAN_readMessageWithID(CANA_BASE,
                                      cana_isr_status,
                                      &frameType,
                                      &msgID,
                                      cana_rx_data);
        // 处理不同 ID 的接收数据
        if (msgID == 0x10)
        {
            if(cana_rx_data[0]==1)
            start_flag=1;
            else start_flag=0;
            // 处理来自ID=0x100的消息
        }
        else if (msgID == 0x11)
        {
            if(cana_rx_data[0]==1)
            start_flag=0;
            stop_flag=1;
            // 处理来自ID=0x200的消息
        }
        else if (msgID == 0x12)
        {
            SpeedRef=((cana_rx_data[1]<<8)& 0xFF00)+(cana_rx_data[0]&0x00FF);
        }

        else if (msgID == 0x13)
        {
            VqTesting=0.01*(((cana_rx_data[1]<<8)& 0xFF00)+(cana_rx_data[0]&0x00FF));
        }
        // 接收数据
        //CAN_readMessage(CANA_BASE, CANA_RX_MSG_OBJ_ID, cana_rx_data);
        /////  必须要清除中断   ！！！！！！！！
        CAN_clearInterruptStatus(CANA_BASE, cana_isr_status);
        cana_rx_count++;
        cana_error_flag = 0;
    }

        else
        {
            // 处理意外中断（可选）
        }
    // 清除 CAN 全局中断状态
    CAN_clearGlobalInterruptStatus(CANA_BASE, CAN_GLOBAL_INT_CANINT0);
    // PIE Group 9 中断应答
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

