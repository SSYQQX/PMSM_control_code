#include "bsp_can.h"

//==========================
// 全局变量定义
//==========================

// CANA发送缓冲区，初始值为示例数据
uint16_t cana_tx_data[4] = {0x12, 0x34, 0x56, 0x78};

// CANA接收缓冲区
uint16_t cana_rx_data[4] = {0};
//CAN接收变量
CAN_MsgFrameType frameType=CAN_MSG_FRAME_STD;
uint32_t msgID=0;//消息ID

// CANA发送计数器（用于统计成功发送次数）
volatile uint32_t cana_tx_count = 0;

// CANA接收计数器（用于统计成功接收次数）
volatile uint32_t cana_rx_count = 0;

// CANA错误标志位（中断中检测到错误后置1）
volatile uint32_t cana_error_flag = 0;

uint32_t cana_isr_status=0;
//==========================
// CAN 初始化函数
//==========================
void bsp_can_init(void)
{
    //----------------------------
    // 1. 配置 CANA 所用的 GPIO 引脚
    //----------------------------
    GPIO_setPinConfig(DEVICE_GPIO_CFG_CANRXA);  // 设置 CANA RX 引脚
    GPIO_setPinConfig(DEVICE_GPIO_CFG_CANTXA);  // 设置 CANA TX 引脚

    //----------------------------
    // 2. 初始化 CANA 模块
    //----------------------------
    CAN_initModule(CANA_BASE);  // 初始化 CAN 控制器

    // 设置 CANA 位率为 500 kbps，时钟源为 SYSCLK，时间段为 20 个时间量子
    CAN_setBitRate(CANA_BASE, DEVICE_SYSCLK_FREQ, 1000000, 20);

    //----------------------------
    // 3. 启用中断功能
    //----------------------------
    // 启用接收中断（IE0）和错误中断
    CAN_enableInterrupt(CANA_BASE, CAN_INT_IE0 | CAN_INT_ERROR);

    // 启用 CAN 控制器的全局中断功能
    CAN_enableGlobalInterrupt(CANA_BASE, CAN_GLOBAL_INT_CANINT0);

    //----------------------------
    // 4. 配置消息对象
    //----------------------------

    ////////邮箱配置，可以有32个邮箱号
    // 配置 TX 消息对象：标准帧、发送类型、启用发送中断
    CAN_setupMessageObject(CANA_BASE,
                           CANA_TX_MSG_OBJ_ID,   // 消息对象编号
                           0x1,                  // 消息 ID
                           CAN_MSG_FRAME_STD,    // 标准帧格式
                           CAN_MSG_OBJ_TYPE_TX,  // 发送类型
                           0,                    // 无掩码
                           0, // 启用发送中断
                           CANA_MSG_DATA_LENGTH  // 数据长度（单位：字节）
    );

    // 配置 RX 消息对象：标准帧、接收类型、启用接收中断
    CAN_setupMessageObject(CANA_BASE,
                           CANA_RX_MSG_OBJ_ID,
                           0x2,                  // 消息 ID
                           CAN_MSG_FRAME_STD,
                           CAN_MSG_OBJ_TYPE_RX,
                           0,
                           CAN_MSG_OBJ_RX_INT_ENABLE,
                           CANA_MSG_DATA_LENGTH
    );


    // 配置 RX 消息对象：标准帧、接收类型、启用接收中断
    CAN_setupMessageObject(CANA_BASE,
                           CANA_RX_MSG_OBJ_Start_ID,
                           0x10,                  // 消息 ID
                           CAN_MSG_FRAME_STD,     // 标准帧格式
                           CAN_MSG_OBJ_TYPE_RX,   // 接收类型
                           0,                     // 无掩码
                           CAN_MSG_OBJ_RX_INT_ENABLE,//启用接收中断
                           1   //数据长度
    );


    CAN_setupMessageObject(CANA_BASE,
                           CANA_RX_MSG_OBJ_Stop_ID,
                           0x11,                  // 消息 ID
                           CAN_MSG_FRAME_STD,     // 标准帧格式
                           CAN_MSG_OBJ_TYPE_RX,   // 接收类型
                           0,                     // 无掩码
                           CAN_MSG_OBJ_RX_INT_ENABLE,//启用接收中断
                           1   //数据长度
    );

    CAN_setupMessageObject(CANA_BASE,
                           CANA_RX_MSG_OBJ_SpeedRef_ID,
                           0x12,                  // 消息 ID
                           CAN_MSG_FRAME_STD,     // 标准帧格式
                           CAN_MSG_OBJ_TYPE_RX,   // 接收类型
                           0,                     // 无掩码
                           CAN_MSG_OBJ_RX_INT_ENABLE,//启用接收中断
                           2   //数据长度
    );

    CAN_setupMessageObject(CANA_BASE,
                           CANA_RX_MSG_OBJ_VqTesting_ID,
                           0x13,                  // 消息 ID
                           CAN_MSG_FRAME_STD,     // 标准帧格式
                           CAN_MSG_OBJ_TYPE_RX,   // 接收类型
                           0,                     // 无掩码
                           CAN_MSG_OBJ_RX_INT_ENABLE,//启用接收中断
                           2   //数据长度
    );

    //----------------------------
    // 5. 启动 CAN 模块
    //----------------------------
    CAN_startModule(CANA_BASE);

    //----------------------------
    // 6. 注册并启用中断服务函数
    //----------------------------
    Interrupt_register(INT_CANA0, &CANA_ISR); // 将 ISR 注册给 INT_CANA0 中断向量
    Interrupt_enable(INT_CANA0);              // 启用中断
}

//==========================
// CANA 发送函数
// @param data：要发送的数据指针
// @param len：发送字节数（最大不能超过 CANA_MSG_DATA_LENGTH）
//==========================
void bsp_can_send(uint16_t *data, uint16_t len,uint32_t MSG_OBJ_ID)
{
    // 若发送长度超出限制，则裁剪为最大允许长度
    if (len > CANA_MSG_DATA_LENGTH) len = CANA_MSG_DATA_LENGTH;

    // 调用驱动库函数发送数据
    CAN_sendMessage(CANA_BASE, MSG_OBJ_ID, len, data);
}


void bsp_can_send_float(float value, uint32_t msgID,uint32_t MSG_OBJ_ID)
{
    // 使用 union 获取 float 对应的 32 位二进制值
    //union 是一种 特殊的数据结构，它可以在 同一块内存空间 中存储不同类型的数据，但任意时刻只能存一个有效值。
    union {
        float f;
        uint32_t i32;
    } converter;

    converter.f = value;

    // 将 32 位整数拆为 4 个字节，并分别放入 4 个 Uint16（低8位有效，高8位为0）
    uint16_t msgData[4];
    msgData[0] = (uint16_t)((converter.i32 >> 0)  & 0xFF);  // Byte 0
    msgData[1] = (uint16_t)((converter.i32 >> 8)  & 0xFF);  // Byte 1
    msgData[2] = (uint16_t)((converter.i32 >> 16) & 0xFF);  // Byte 2
    msgData[3] = (uint16_t)((converter.i32 >> 24) & 0xFF);  // Byte 3

    // 配置发送消息对象，使用标准帧 + 可变 ID
    CAN_setupMessageObject(CANA_BASE,
                           MSG_OBJ_ID,         // 消息对象编号
                           msgID,                      // 帧 ID
                           CAN_MSG_FRAME_STD,          // 标准帧格式
                           CAN_MSG_OBJ_TYPE_TX,        // 发送类型
                           0,                          // 不使用掩码
                           0,  // 启用发送中断CAN_MSG_OBJ_TX_INT_ENABLE,不启用 0
                           4);                         // 数据长度 = 4 字节

    // 发送数据（注意：只发送低 8 位，每个 Uint16 的高 8 位会被忽略）
    bsp_can_send(msgData, 4,MSG_OBJ_ID);  // len 单位是字节
}

void bsp_can_send_two_floats(float f1, float f2, uint32_t msgID,uint32_t MSG_OBJ_ID)
{
    union {
        float f;
        uint32_t i32;
    } conv1, conv2;

    conv1.f = f1;
    conv2.f = f2;

    // 每个 float 拆成 4 个 Uint16，共 8 个字节（低8位有效）
    uint16_t msgData[8];

    // 第一个 float → byte0 ~ byte3
    msgData[0] = (uint16_t)((conv1.i32 >> 0)  & 0xFF);
    msgData[1] = (uint16_t)((conv1.i32 >> 8)  & 0xFF);
    msgData[2] = (uint16_t)((conv1.i32 >> 16) & 0xFF);
    msgData[3] = (uint16_t)((conv1.i32 >> 24) & 0xFF);

    // 第二个 float → byte4 ~ byte7
    msgData[4] = (uint16_t)((conv2.i32 >> 0)  & 0xFF);
    msgData[5] = (uint16_t)((conv2.i32 >> 8)  & 0xFF);
    msgData[6] = (uint16_t)((conv2.i32 >> 16) & 0xFF);
    msgData[7] = (uint16_t)((conv2.i32 >> 24) & 0xFF);

    // 配置发送消息对象（不启用中断）
    CAN_setupMessageObject(CANA_BASE,
                           MSG_OBJ_ID,       // 消息对象编号
                           msgID,                    // 帧 ID
                           CAN_MSG_FRAME_STD,        // 标准帧
                           CAN_MSG_OBJ_TYPE_TX,      // 发送类型
                           0,                        // 不使用掩码
                           0,                        // 不启用中断
                           8);                       // 发送8字节

    // 发送数据（注意数据长度以“字节”为单位）
    bsp_can_send(msgData, 8,MSG_OBJ_ID);
}

