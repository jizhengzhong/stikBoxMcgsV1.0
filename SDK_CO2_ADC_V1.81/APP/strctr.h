
#ifndef STRCTR_H
#define	STRCTR_H

#include <stdint.h>

#include "systask.h"

#ifdef	__cplusplus
extern "C" {
#endif
	
#ifdef   _STR_MODULE_
#define  MY_EXT
#else
#define  MY_EXT  extern
#endif  

void StrCtrTask(void);


typedef enum {
    SAVE_IDLE=0,
    SAVE_NORMAL =  1,
    SAVE_NEWLINE  =  2,
} RunDataSaveReq_type;

MY_EXT struct {
    RunDataSaveReq_type RunDataSaveReq;      //1-继续存储 ,2-64新行且对齐4K边界
    uint32_t RunDataSaveAddr;    //存储数据地址索引，需初始化搜索
    uint8_t RunDataSaveBuff[130]; //运行数据存储缓冲区
} StRunDataSave;

typedef enum {
    READ_IDLE=0,
    READ_DATA  =  1,   //度数据
    READ_RUNCNT = 2,   //读运行次数
    READ_ADDR_N = 3,   //读取第n次数据的首地址
} RunDataReadReq_type;

MY_EXT struct {
    RunDataReadReq_type CmdReq;
    uint32_t Amount;        //读取数据数量
    uint32_t Address;       //请求数据地址
    uint32_t AddressStart;  
    uint32_t RunTimesTotal; //总运行次数
    
    uint8_t *Des;
} StFlashReadCmd;


MY_EXT uint32_t RUN_DATA_NEWLINE_SIZE;      //     必须为2的n次方数且小于等于512
MY_EXT uint32_t RUN_DATA_NORMAL_SIZE;       //     必须为2的n次方数且小于等于RUN_DATA_NEWLINE_SIZE

typedef enum {
    HIS_BUFF_IDLE,
    HIS_BUFF_REQ,     //请求数据状态
    HIS_BUFF_BUSY,    //正在写入数据
    HIS_BUFF_READY,   //数据就绪
    HIS_BUFF_USING,   //正在写出到文件
} HisDataBuffSt_type;

typedef enum {
    HIS_DATA_BUFF_NOUSED=0,
    HIS_DATA_BUFF1_USING=1,
    HIS_DATA_BUFF2_USING=2,
} HisDataBuffUsed_type;

MY_EXT struct {
    volatile uint32_t HisDataReadReq;  //历史数据读取请求 数量
    volatile uint8_t FlashReadReq;
    volatile uint8_t HisDataReadEnd;
    volatile uint8_t NewFileReq;

    uint32_t HisDataStartAddr; //历史数据读取开始位置
    uint32_t HisDataIndexAddr; //历史数据读取当前索引位置
    volatile HisDataBuffSt_type StBuff1;
    volatile HisDataBuffSt_type StBuff2;
    volatile HisDataBuffUsed_type StBuffUsed; //指示正在使用的缓冲区， 连续读时另外一个缓冲区可以用来通过DMA获取数据
    union {
        uint32_t w[1025];
        uint8_t  b[4100];
    } Buff1;
    union {
        uint32_t w[1025];
        uint8_t  b[4100];
    } Buff2;
} StHisDataRead;

#undef MY_EXT

void StrCtrTask(void);



#undef MY_EXT

#ifdef	__cplusplus
}
#endif

#endif	/* STRCTR_H */
