

#include "bsp_timer.h"
#include "cputimer.h"

//timer0属于pie中断，timer1与timer2属于cpu中断，不属于pie中断。

void bsp_timer0_init(void)
{
    EALLOW;
    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    EDIS;

    //InitCpuTimers();

    ConfigCpuTimer(&CpuTimer0, 200, 2000000);/* 周期50us  20KHz */
    CpuTimer0Regs.TCR.all = 0x4000;
    // Enable CPU INT1 which is connected to CPU-Timer 0:
    IER |= M_INT1;
    // Enable TINT0 in the PIE: Group 1 __interrupt 7
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;


    EINT;
    ERTM;
}

void bsp_timer1_init(void)
{
    EALLOW;
    PieVectTable.TIMER1_INT = &cpu_timer1_isr;
    EDIS;

    ConfigCpuTimer(&CpuTimer1, 200, 500000);/* 周期50us  20KHz */
    CpuTimer1Regs.TCR.all = 0x4000;

    IER |= M_INT13;//使能cpu中断13

    EINT;
    ERTM;
}

void bsp_timer2_init(void)
{
    EALLOW;
    PieVectTable.TIMER2_INT = &cpu_timer2_isr;
    EDIS;

    ConfigCpuTimer(&CpuTimer2, 200, 2000000);/* 周期50us  20KHz */
    CpuTimer2Regs.TCR.all = 0x4000;

    IER |= M_INT14;//使能cpu中断13

    EINT;
    ERTM;
}


