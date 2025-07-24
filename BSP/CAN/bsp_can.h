

#ifndef BSP_CAN_H_
#define BSP_CAN_H_

#include "driverlib.h"
#include "device.h"

#include "driverlib.h"
#include "device.h"

// 消息相关定义
#define CANA_TX_MSG_OBJ_ID     1
#define CANA_RX_MSG_OBJ_ID     2
#define CANA_TX_MSG_OBJ_THETA  3
#define CANA_TX_MSG_OBJ_SVGEN  4

#define CANA_RX_MSG_OBJ_Start_ID     10
#define CANA_RX_MSG_OBJ_Stop_ID      11
#define CANA_RX_MSG_OBJ_SpeedRef_ID  12
#define CANA_RX_MSG_OBJ_VqTesting_ID  13


#define CANA_MSG_DATA_LENGTH   8

// 全局变量定义（如果需要外部访问）
extern uint16_t cana_tx_data[4];
extern uint16_t cana_rx_data[4];
extern volatile uint32_t cana_tx_count;
extern volatile uint32_t cana_rx_count;
extern volatile uint32_t cana_error_flag;
extern uint32_t cana_isr_status;
//CAN接收变量
extern CAN_MsgFrameType frameType;
extern uint32_t msgID;//消息ID

// 初始化接口
void bsp_can_init(void);

// 发送函数（阻塞/非阻塞可根据需求拓展）
void bsp_can_send(uint16_t *data, uint16_t len,uint32_t MSG_OBJ_ID);

void bsp_can_send_float(float value, uint32_t msgID,uint32_t MSG_OBJ_ID);
void bsp_can_send_two_floats(float f1, float f2, uint32_t msgID,uint32_t MSG_OBJ_ID);

// 中断服务函数，供外部注册
__interrupt void CANA_ISR(void);


#endif /* BSP_CAN_H_ */
