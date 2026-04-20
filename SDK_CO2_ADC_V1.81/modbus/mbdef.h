/* 
 * File:   mbport.h
 * Author: Administrator
 *
 * Created on 2015年8月24日, 下午8:38
 * modify @ 20180804
 * --微调
 */

#ifndef __MBDEF_H
#define	__MBDEF_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "mbcrc.h"   

typedef unsigned char       mbSlaveIdType;  
typedef unsigned short      mbRegNumType; 
typedef unsigned short      mbRegAddrType; 
typedef unsigned short      mbRegDataType; 

#define MB_MUL_TASK_EN                          0
#define MB_ASCII_EN                             0
    
#define MB_BUFF_SIZE                            (256 )
#define MB_ERR_CNT_PV                           (  5 )
#define SMB_TIME_OUT_MS                         (6000)
#define MMB_TIME_OUT_MS                         (150 )

#define MB_EVENT_SEND_END                       (0x01)
#define MB_EVENT_RCV_END                        (0x02)

#define MB_FUN_READ_COILS_ENABLED               (  1 )  //Fun: 01
#define MB_FUN_READ_DISCRETE_INPUTS_ENABLED     (  1 )  //Fun: 02
#define MB_FUN_READ_REG_ENABLED                 (  1 )  //Fun: 03    
#define MB_FUN_READ_INPUT_ENABLED               (  1 )  //Fun: 04
#define MB_FUN_WRITE_COIL_ENABLED               (  1 )  //Fun: 05
#define MB_FUN_WRITE_REG_ENABLED                (  1 )  //Fun: 06
#define MB_FUN_WRITE_MULTIPLE_COILS_ENABLED     (  1 )  //Fun: 15
#define MB_FUN_WRITE_MULTIPLE_REG_ENABLED       (  1 )  //Fun: 16 
#define MB_FUN_W_R_MULTIPLE_REG_ENABLED         (  1 )  //Fun: 0x17 

#define MB_REG_COILS_CB_ENABLE	    ( MB_FUN_READ_COILS_ENABLED + MB_FUN_WRITE_COIL_ENABLED + MB_FUN_WRITE_MULTIPLE_COILS_ENABLED  ) 
#define MB_REG_HOLDING_CB_ENABLE    ( MB_FUN_READ_REG_ENABLED + MB_FUN_WRITE_REG_ENABLED + MB_FUN_WRITE_MULTIPLE_REG_ENABLED + MB_FUN_W_R_MULTIPLE_REG_ENABLED )  
#define MB_REG_INPUT_CB_ENABLE	    ( MB_FUN_READ_INPUT_ENABLED   ) 
#define MB_REG_DISCRETE_CB_ENABLE   ( MB_FUN_READ_DISCRETE_INPUTS_ENABLED   )    

#define MB_COIL_ON                      0xFF00
#define MB_COIL_OFF                     0x0000

#define MB_ERR_CODE                     0x80
#define MB_FAULT_CODE1                  0x01
#define MB_FAULT_CODE2                  0x02
#define MB_FAULT_CODE3                  0x03
#define MB_FAULT_CODE4                  0x04

#define     MB_ERR_NONE        0        //!< no error. 
#define     MB_ERR_NOREG       1        //!< illegal register address. 
#define     MB_ERR_DATA        2 
#define     MB_ERR_CRC_LRC     3 
#define     MB_ERR_FUN_CODE    4
#define     MB_ERR_HAL         5
#define     MB_ETIMEDOUT       16       

#define     MB_NULL            (0)

    /* -----------------------  Cmd Type definitions ---------------------------------*/
    typedef enum {
        FUN_CODE_NONE = 0,
        FUN_CODE_READ_COILS = 0x01,
        FUN_CODE_READ_DISCRETE = 0x02,
        FUN_CODE_READ_REG = 0x03,
        FUN_CODE_READ_INPUT = 0x04,
        FUN_CODE_WRITE_COIL = 0x05,
        FUN_CODE_WRITE_REG = 0x06,
        FUN_CODE_WRITE_MULTIPLE_COIL = 0x0F,
        FUN_CODE_WRITE_MULTIPLE_REG = 0x10,
        FUN_CODE_WRITE_MASK = 0x16,
        FUN_CODE_W_R_MULTIPLE_REG = 0x17,
        FUN_CODE_READ_SLAVE_ID = 0x2B,
        FUN_CODE_DUGSII_REG_WRITE = 0x80, //CRC 校验的开启/关闭由配置文件的 0x2C.6 位控制
        FUN_CODE_DUGSII_REG_READ = 0x81,
        FUN_CODE_DUGSII_PV_WRITE = 0x82,
        FUN_CODE_DUGSII_PV_READ = 0x83,
    } mbCmdType;

    /* -----------------------  Data Type definitions -------------------------*/
    typedef union {
        uint16_t Val;
        uint16_t uVal;
        int16_t iVal;
        struct {
            uint8_t LB;
            uint8_t HB;
        } byte;
    } mbU16Type;

    /* -----------------------  Receive State Type definitions ----------------*/
    typedef enum {
        MB_RX_IDLE, // Receiver is in idle state.
        MB_RX_RCV, // Frame is beeing received.
        MB_RX_RCVEND, // Port Receive complete ,3.5T timer
        MB_RX_WAIT, //Just for master
        MB_RX_TIMEOUT // Time out ,Send a command and no data return after 500ms( --> system timer)
    } mbRcvStType;

    /* -----------------------  Send State Type definitions -------------------*/
    typedef enum {
        MB_TX_IDLE, // Transmitter is in idle state.
        MB_TX_XMIT // Transmitter is in transfer state.
    } mbSndStType;

    /* -----------------------  Data parity definitions -----------------------*/
    typedef enum {
        MB_PAR_NONE, /*!< No parity. */
        MB_PAR_ODD, /*!< Odd parity. */
        MB_PAR_EVEN /*!< Even parity. */
    } mbParityType;

    /* -----------------------  RW Type definitions ---------------------------*/
    typedef enum {
        MB_REG_READ,
        MB_REG_WRITE,
    } mbRegModeType;

    /* --------------------  Modbus mode Type definitions ---------------------*/
    typedef enum {
        MB_RTU_MASTER,
        MB_RTU_SLAVE,
        //ASCII_MASTER,
        //ASCII_SLAVE
    } mbModeType;

    /* ------------------  baudary Type definitions ---------------------------*/
    typedef enum {
        MB_BAUD_1200 = 0,
        MB_BAUD_2400 = 1,
        MB_BAUD_4800 = 2,
        MB_BAUD_9600 = 3,
        MB_BAUD_19200 = 4,
        MB_BAUD_38400 = 5,
        MB_BAUD_115200 = 6
    } mbBaudType;

    /* ------------------  Run State Type definitions -------------------------*/
    typedef union {
        uint64_t St;

        struct {
            uint8_t MasterMode;
            uint8_t RTUMode;
            uint8_t PortSt;
            uint8_t Connect;
        } bits;
    } mbRunStType;

    /* ------------------  Error State Type definitions -----------------------*/
    typedef union {
        uint64_t St;

        struct {
            uint8_t ErrHal ;
            uint8_t ErrData ;
            uint8_t ErrNoReg ;
            uint8_t ErrFunCode ;
            uint8_t ErrCrcLrc ;
        } bits;
    } mbErrStType;

    /* ---------------  Cmd Buffer State Type definitions ---------------------*/
    typedef enum {
        MB_CMD_ST_IDLE = 0, //命令无效或已经执行过
        MB_CMD_ST_READY = 1, //命令就绪准备执行
    } mbCmdStType;

    /* ------------- Master Cmd Buffer struct Type definitions -----------------------*/
    typedef struct {  
        uint8_t DeviceId; //从机ID
        mbCmdType Cmd; //命令  //
        uint16_t *Data; //数据地址
        uint16_t mbAddr; //modbus地址
        uint16_t amount; //数量
        mbCmdStType ReqSt; //命令状态
        void (*CallBack)(uint8_t CmdSite, uint8_t ErrSt); //回调函数
    } mbCmdBuffType;

    /* ------------ Slave extern Function Type definitions --------------------*/
    typedef struct {
        uint8_t(*RegInputCB)(uint8_t * pucRegBuffer, uint16_t usAddress, uint16_t usNRegs);
        uint8_t(*RegHoldingCB)(uint8_t * pucRegBuffer, uint16_t usAddress, uint16_t usNRegs, mbRegModeType eMode);
        uint8_t(*RegDiscreteCB)(uint8_t *pucRegBuffer, uint16_t usAddress, uint16_t usNCoils);
        uint8_t(*RegCoilsCB)(uint8_t * pucRegBuffer, uint16_t usAddress, uint16_t usNCoils, mbRegModeType eMode);
    } mbSlaveFunType;

    /* ------------- Master Function Type definitions -------------------------*/
    typedef struct {
        uint8_t ErrCnt; //错误计数器 //发送新命令时自动清除一次
        uint8_t DcmdReqIndex; //动态命令请求索引
        uint8_t DcmdHandleIndex; //动态命令处理索引
        uint8_t ScmdHandleIndex; //静态命令处理索引
        uint8_t CmdHandleIndex; //当前处理命令索引
        uint8_t CmdBuffSize; //总缓冲池尺寸
        uint8_t ScmdBuffSize; //静态缓冲池尺寸
        uint8_t LenReq; //请求数据长度 字节数
        uint16_t *pData; //数据指针 - 存放处理过的数据
        mbCmdBuffType* pcmdBuff; //指令指针 - 指向主机命令缓冲池
#if MB_MUL_TASK_EN > 0 
        void (*MutexPend)(void);
        void (*MutexPost)(void);
#endif
    } mbMasterFunType;

    /* -------------- Modbus object Type definitions ---------------------------*/
    typedef struct {
        /*运行状态*/
        mbRunStType RunSt; //运行状态
        mbErrStType ErrSt; //错误状态
        mbRcvStType RcvSt; //接收状态       //硬件中断管理 //接收到字符设为RCV ，接收结束设为RCVEND //有操作系统时由外部设为TIMEOUT
        mbSndStType SndSt; //发送状态       //报文发送结束由外部设为IDLE
        /*定时器管理*/
        uint8_t RtuTimerCnt; //RTU结束判定计时器，用于软件定时
        uint8_t RtuTimerSv;  //RTU结束判定设定值，用于软件定时
        uint8_t bRtuTimerOn; //RTU定时器开启
        uint8_t RtuRcvendKeepCnt; //RTU结束 //无OS时可用于做RCVEND延时处理的判定 //针对主机模式，从机无需延时，主机延时可以提供足够的间隔供其他不匹配从机复位状态
        uint8_t RtuRcvendDelaySet; //接收处理延时设定，带OS时设为0即可 //OS主机模式在发送函数前添加延时，如果确定只有一个从机时，也可以省略延时
        /*过程参数*/
        uint8_t SlaveId; //从机id //仅在从模式设置；主模式由内部管理，禁止应用层修改。
        uint16_t TimeOutCnt; //本机报文发送结束后开始计时//仅供主机
        uint16_t RcvCnt; //接收数据个数   //硬件中断及应用层管理 
        uint16_t SndSize; //待发送数据个数
        uint16_t SndCnt; //发送过程计数
        /*事件定义*/
        uint16_t os_rtu_end_event; //RTU结束事件值 预设 //使用OS且用内部管理定时器时使用
        uint16_t os_timeout_event; //RTU超时事件值 预设 //使用OS且用内部管理定时器时使用
        /*状态监视*/
        uint16_t success_keep_cnt; //通信成功保持次数
        uint16_t fail_keep_cnt;   //通信失败保持次数
        uint32_t success_cnt;      //通信成功总次数
        uint32_t fail_cnt;        //通信失败总次数
        uint16_t active_keep_cnt; //活动状态计时 //包括 MB_TX_XMIT MB_RX_RCV 
        uint16_t active_keep_max; //活动状态计时最大值，用于调试监控
        uint16_t idle_time_cnt;   //空闲计时器，RX,TX均为IDLE时计时
        /*回调函数*/
        void (*AduSend)(void); //函数指针 - 发送AduBuff数据到端口，数量由SndSize指示
        void (*os_event_send)(uint16_t event); //使用OS是发送事件
        void (*RcvEndHandleComp)(uint8_t ErrSt);
        /*功能接口*/
        union {
            mbSlaveFunType Slave; //从机结构
            mbMasterFunType Master; //主机结构
        } Fun;
        uint8_t AduBuff[MB_BUFF_SIZE]; //数据缓冲
    } mbObjType;

    /* -------------- 状态机过程 ---------------------------*/
    /*
    从机：
        RX-IDLE&TX-IDLE -> RX-RCV&TX-IDLE -> RX-RCVEND&TX-IDLE -> RX-IDLE&TX-SEND -> RX-IDLE&TX-IDLE 
             
    主机：
        RX-IDLE&TX-IDLE -> RX-RCV&TX-SEND -> RX-RCV&TX-IDLE -> RX-RCVEND&TX-IDLE -> RX-IDLE&TX-IDLE 
    
    为尽量兼容其他设备，接收处理施加延时：
    
    对于含操作系统的应用，可以在数据发送函数添加任务延时即可。
    
    对于不含操作系统的应用--
    主机模式在接收结束后延时处理即可。
    从机接收到非本机地址直接处理，接收到本地地址延时处理。
    */

#ifdef	__cplusplus
}
#endif

#endif	/* __MBDEF_H */

