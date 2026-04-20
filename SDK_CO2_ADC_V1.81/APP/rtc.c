#include "osObjects.h"
#include "rtc.h"
#include "pincfg.h"
#include "dataproc.h"
#include "systask.h"
#include "mbapp.h"
#include "partab.h"

RTC_HandleTypeDef RTC_Handler;  
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

static void Error_Handler(void);
static void Screen_Read_Time_Set(void);
static void Appoint_Function(void);

void RtcTask(void const *argument)
{
  for(;;)
  {
   
   HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
   HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
   Appoint_Function();
 //  Screen_Read_Time_Set();

//  if(HAL_RTCEx_BKUPRead(&RTC_Handler,RTC_BKP_DR0)!=0X5050)
   if( ParTab[PAR_POS_SYS_CLOCK_FLAG].uVal == 1 )
    { 
      Screen_Read_Time_Set();
      RTC_Set_Time(ParTab[PAR_POS_SET_HOUR].uVal,ParTab[PAR_POS_SET_MINU].uVal,0,RTC_HOURFORMAT12_PM);	        
      RTC_Set_Date(ParTab[PAR_POS_SET_YEAR].uVal-2000,ParTab[PAR_POS_SET_MONTH].uVal,ParTab[PAR_POS_SET_DAY].uVal,2);		                    
//      HAL_RTCEx_BKUPWrite(&RTC_Handler,RTC_BKP_DR0,0X5050);
    }        
    osDelay(5);
  
  }
}


static void Screen_Read_Time_Set(void)
{
  
 RTC_DateStruct.Year  = ParTab[PAR_POS_SET_YEAR].uVal-2000;
 RTC_DateStruct.Month = ParTab[PAR_POS_SET_MONTH].uVal  ;        
 RTC_DateStruct.Date  = ParTab[PAR_POS_SET_DAY].uVal ;   
 RTC_TimeStruct.Hours = ParTab[PAR_POS_SET_HOUR].uVal   ;
 RTC_TimeStruct.Minutes = ParTab[PAR_POS_SET_MINU].uVal;  

}

static void Appoint_Function(void)
{

   if( ParTab[PAR_POS_SET_APPOIN_YEAR].uVal  == ( RTC_DateStruct.Year + 2000 )  &&
       ParTab[PAR_POS_SET_APPOIN_MONTH].uVal == RTC_DateStruct.Month        &&  
       ParTab[PAR_POS_SET_APPOIN_DAY].uVal   == RTC_DateStruct.Date         &&   
       ParTab[PAR_POS_SET_APPOIN_HOUR].uVal  == RTC_TimeStruct.Hours        && 
       ParTab[PAR_POS_SET_APPOIN_MINU].uVal  == RTC_TimeStruct.Minutes   
       )
   {
     SysRunSt.bSysRun = 1; 
   }


}

HAL_StatusTypeDef RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	RTC_TimeStructure.Hours=hour;
	RTC_TimeStructure.Minutes=min;
	RTC_TimeStructure.Seconds=sec;
	RTC_TimeStructure.TimeFormat=ampm;
	RTC_TimeStructure.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
        RTC_TimeStructure.StoreOperation=RTC_STOREOPERATION_RESET;
	return HAL_RTC_SetTime(&RTC_Handler,&RTC_TimeStructure,RTC_FORMAT_BIN);	
}

//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
HAL_StatusTypeDef RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
	RTC_DateTypeDef RTC_DateStructure;
    
	RTC_DateStructure.Date=date;
	RTC_DateStructure.Month=month;
	RTC_DateStructure.WeekDay=week;
	RTC_DateStructure.Year=year;
	return HAL_RTC_SetDate(&RTC_Handler,&RTC_DateStructure,RTC_FORMAT_BIN);
}



/* RTC init function */
void MX_RTC_Init(void)
{

    /**Initialize RTC Only
     */
    RTC_Handler.Instance = RTC;
    RTC_Handler.Init.HourFormat = RTC_HOURFORMAT_24;
    RTC_Handler.Init.AsynchPrediv = 127;
    RTC_Handler.Init.SynchPrediv = 255;
    RTC_Handler.Init.OutPut = RTC_OUTPUT_DISABLE;
    RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTC_Handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&RTC_Handler) != HAL_OK)
    {
        Error_Handler();
    }	 

}

static void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
 while(1);
    /* USER CODE END Error_Handler_Debug */
}



