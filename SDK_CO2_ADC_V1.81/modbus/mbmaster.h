/* 
 * File:   MBMaster.h
 * Author: Administrator
 * 
 * Created on 2015年5月20日, 下午7:31
 */
/*----------------------------------------------------------------------------*/
/*---------------对于16位数据，高8位在前，低8位在后-----------------------------*/
/*----------------------------------------------------------------------------*/

#ifndef MBMASTER_H
#define	MBMASTER_H

#include "mbdef.h"

#ifdef	__cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////                                            主机底层接口                                           //////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Fun: 0x01
uint8_t    mbReadCoils(mbObjType * mbObj, uint16_t DataAdd, uint16_t Len);

//Fun: 0x02
uint8_t    mbReadDiscreteInputs(mbObjType * mbObj, uint16_t DataAdd, uint16_t Len);

//Fun: 0x03
uint8_t    mbReadRegister(mbObjType * mbObj, uint16_t DataAdd, uint16_t Len);

//Fun: 0x04
uint8_t    mbReadInputRegister(mbObjType * mbObj, uint16_t DataAdd, uint16_t Len);

//Fun: 0x05
uint8_t    mbWriteCoil(mbObjType * mbObj, uint16_t DataAdd, uint16_t Data);

//Fun: 0x06
uint8_t    mbWriteRegister(mbObjType * mbObj, uint16_t DataAdd, uint16_t Data);

//Fun: 0x0F
uint8_t    mbWriteMultipleCoils(mbObjType * mbObj, uint16_t DataAdd, uint16_t Len);

//Fun: 0x10
uint8_t    mbWriteMultipleRegister(mbObjType * mbObj, uint16_t DataAdd, uint16_t Len);

//Fun: 0x17
uint8_t    mbWRRegister(mbObjType * mbObj, uint16_t rDataAdd, uint16_t rLen, uint16_t wDataAdd, uint16_t wLen, uint16_t * pWdata);

uint8_t mbReadRegProc(mbObjType * mbObj);
uint8_t mbReadCoilProc(mbObjType * mbObj);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////                                            主机应用层接口                                         //////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-----------------------------------------------------------------------------------------------------------------------------*
 *    主机使用缓冲池来存放用户请求的命令，Poll函数自动处理缓冲池中的命令
 *
 *    缓冲池结构如下
 *        0                                                      mbSCBuffSize                                          mbCBuffSize
 *        |                                                           |                                                     |
 *    | Buff0 | Buff1 | Buff2 |  ....  | Buff[mbSCBuffSize-1] | Buff[mbSCBuffSize] | Buff[mbSCBuffSize+1] |  ....  | Buff[mbCBuffSize]|
 *        |                                       |                   |                                                     |
 *       静态                                    静态                动态                                                  动态
 *       缓冲                                    缓冲                缓冲                                                  缓冲
 *       起始                                    结束                开始                                                  结束
 *
 *    扫描顺序：
 *    先扫描动态区，从上次处理的位置开始，命中后退出。若扫描完整个动态区都没有命中，开始扫描静态区                               
 *    扫描静态区，方法同动态区。
 *    两个区域的区别在于：动态区的命令处理后会被清除，而静态一直保持就绪状态。
 *
 *
 *----------------------------------------------------------------------------------------------------------------------------*/
//缓冲池初始化
void mbCmdBuffInit(mbObjType *mbObj, mbCmdBuffType * pBuff, uint8_t mbCBuffSize, uint8_t mbSCBuffSize);
//信号量设置 //初始化之后才可以调用 //不需要时可以不调用 //设为MB_NULL取消功能
#if MB_MUL_TASK_EN > 0 
void mbCmdMutexInit(mbObjType *mbObj, void (*Pend)(void),void (*Post)(void));
#endif

//动态命令请求 
uint8_t mbStaticCmdSet(mbObjType *mbObj, uint8_t CmdSite, uint8_t DeviceId, mbCmdType Cmd, uint16_t *Data, uint16_t mbAddr, uint16_t amount, void (*CallBack)(uint8_t,uint8_t));
//静态命令设置   //将命令码设为0表示取消该命令
uint8_t mbDynaCmdReq(mbObjType *mbObj, uint8_t DeviceId, mbCmdType Cmd, uint16_t *Data, uint16_t mbAddr, uint16_t amount, void (*CallBack)(uint8_t,uint8_t));

//下面两个函数功能同上两个一样，但不申请互斥信号量
uint8_t mbStaticCmdSetNoOS(mbObjType *mbObj, uint8_t CmdSite, uint8_t DeviceId, mbCmdType Cmd, uint16_t *Data, uint16_t mbAddr, uint16_t amount, void(*CallBack)(uint8_t, uint8_t));
uint8_t mbDynaCmdReqNoOS(mbObjType *mbObj, uint8_t DeviceId, mbCmdType Cmd, uint16_t *Data, uint16_t mbAddr, uint16_t amount, void(*CallBack)(uint8_t, uint8_t));

//命令0x17（同时读写）仅支持静态分配，且占用两个连续的缓冲区，第一为读参数，第二为写参数,公有参数以第一个缓冲为准。
//两个缓冲区必须通过mbStaticCmdSetNoOS()申请，如果在需要任务中操作，应手动申请信号量。

//动态缓冲池就绪数量
uint8_t NumOfMmbCmdReady(mbObjType *mbObj);

//动态缓冲池空闲数量
uint8_t NumOfMmbCmdIdle(mbObjType *mbObj);

//清除所有命令状态
void mbCmdStClrAll(mbObjType *mbObj);

//获取当前正在处理的命令
uint32_t mbCmdHandleGet(mbObjType *mbObj, mbCmdBuffType * pBuff);

//主机处理 循环中调用
void mbMasterPoll(mbObjType *mbObj);            

#ifdef	__cplusplus
}
#endif

#endif	/* MBMASTER_H */

