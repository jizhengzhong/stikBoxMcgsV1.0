#ifndef _MB_COM_H
#define _MB_COM_H

#include "mbdef.h"
void mbPackageSend( mbObjType * mbObj);

//以下函数可以在外部实现以提高效率
void mbRtuTimerRun(mbObjType *mbObj); //1ms中断中调用

//使用硬件定时器时不调用这两个函数，有外部重新实现为硬件定时
void mbTimerEnable(mbObjType *mbObj); //接收中断调用
void mbTimerDisable(mbObjType *mbObj);

void mb_obj_init(mbObjType *mbObj);
void mb_poll(mbObjType *mbObj);

#endif
