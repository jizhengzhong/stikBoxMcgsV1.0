#define _LED_MODULE_

#include "osObjects.h"
#include "ledctr.h"
#include "pincfg.h"
#include "dataproc.h"
#include "systask.h"
#include "mbapp.h"
#include "partab.h"
#include "rtc.h"
#include "main.h"
#include "string.h "
#include <stdio.h>

#define PWMTIMER              TIM2
#define PWM_PERIOD            10000
#define PWM_DUTY_MIN          4
#define PWM_DUTY_MAX          9900

#define AT_RESP_END_OK                 "OK"
#define AT_RESP_END_ERROR              "ERROR"
#define AT_RESP_END_AT_READY            "AT Ready"
#define AT_END_CR_LF                   "\r\n"

UART_HandleTypeDef *pAtUart = &huart2;
uint16_t DutyList[512] = { 0 };
uint16_t DutyIndex = 0;
led_breath_type LedBreathObj;


char testsprintf1[12]="\0";
char testsprintf2[12]="\0";
char testsprintf3[12]="\0";
char testsprintf4[12]="\0";
char testsprintf5[12]="\0";
char testsprintf6[12]="\0";
char testsprintf7[12]="\0";
char testsprintf8[12]="\0";
char testsprintf9[12]="\0";
char testsprintf10[12]="\0";


void led_init(void);
void LedCtr(void);

void HexTOTelephoneStr(uint16_t *pBUF,char *pDATA)
{
  char i;
  for(i=0;i<6;i++)
  {
    pDATA[2*i]   = pBUF[i]>>8;
    pDATA[2*i+1] = (char)pBUF[i];
  }
  pDATA[11]='\0';
}

/**
 *pdata: pointer of data for send
 *len:  len of data to be sent
 *return: the len of data send success
 * @brief hal api for at data send
 */
int at_send_data(uint8_t *pdata, uint16_t len,uint16_t timeout,char *pString)
{
	if(HAL_OK == HAL_UART_Transmit(pAtUart, pdata, len, 0xFFFF))
        {
            TelephoneDefine.timeout=0;
            while(TelephoneDefine.timeout++<timeout)
              {
                    HAL_Delay(10);
                   if( strstr(TelephoneDefine.received_house, pString))
                     {
                         TelephoneDefine.house_num = 0;
                        memset(TelephoneDefine.received_house,0,sizeof(TelephoneDefine.received_house));
                        break;
                     } 
                }	
         return len;
        }
	else
	{
          return 0;
	}

}
uint16_t Send_String(uint8_t *pdata, uint16_t len)
{
	if(HAL_OK == HAL_UART_Transmit(pAtUart, pdata, len, 0xFFFF))
        {
	
         return len;
        }
	else
	{
          return 0;
	}


}
char strbuf[140]="\0";
char *pTelephoneString=strbuf;
void TelephoneConnectString(void)
{


  sprintf((char *)strbuf, "AT+REVNUM=\"");
  pTelephoneString = strcat(strbuf,testsprintf1);
  
  if(testsprintf2[10]!='\0')
    {
      pTelephoneString = strcat(pTelephoneString,";"); 
      pTelephoneString = strcat(pTelephoneString,testsprintf2);
    }
    if(testsprintf3[10]!='\0')
    {
      pTelephoneString = strcat(pTelephoneString,";"); 
      pTelephoneString = strcat(pTelephoneString,testsprintf3); 
    }
   if(testsprintf4[10]!='\0')
   {
      pTelephoneString = strcat(pTelephoneString,";"); 
      pTelephoneString = strcat(pTelephoneString,testsprintf4);
   }
   if(testsprintf5[10]!='\0')
  {
      pTelephoneString = strcat(pTelephoneString,";");
      pTelephoneString = strcat(pTelephoneString,testsprintf5); 
  }
   if(testsprintf6[10]!='\0')
   {
      pTelephoneString = strcat(pTelephoneString,";"); 
      pTelephoneString = strcat(pTelephoneString,testsprintf6); 
   }
   if(testsprintf7[10]!='\0')
   {
      pTelephoneString = strcat(pTelephoneString,";"); 
      pTelephoneString = strcat(pTelephoneString,testsprintf7);
   }
   if(testsprintf8[10]!='\0')
   {
      pTelephoneString = strcat(pTelephoneString,";"); 
      pTelephoneString = strcat(pTelephoneString,testsprintf8); 
   }
   if(testsprintf9[10]!='\0')
   {
      pTelephoneString = strcat(pTelephoneString,";"); 
      pTelephoneString = strcat(pTelephoneString,testsprintf9);
   }
   if(testsprintf10[10]!='\0')
   {
      pTelephoneString = strcat(pTelephoneString,";"); 
      pTelephoneString = strcat(pTelephoneString,testsprintf10); 
   }
  
  pTelephoneString = strcat(pTelephoneString,"\",0,0\r\n");

}
/*****************************************************************************//*!
*
* @brief   led task.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void LedCtrTask(void const *argument)
{
    led_init();
    LedBreathObj.IncCoef = 220; //时间越大呼吸约慢
    LedBreathObj.min = PWM_DUTY_MIN;
    LedBreathObj.max = PWM_DUTY_MAX;
    for (;;)
    {
        osDelay(10);
        LedCtr();
        if( ParTab[PAR_POS_TEL_CONFIRM_].uVal )
        {
          HexTOTelephoneStr(&ParTab[PAR_POS_TEL2_0].uVal, testsprintf2);  
          HexTOTelephoneStr(&ParTab[PAR_POS_TEL1_0].uVal, testsprintf1); 
          HexTOTelephoneStr(&ParTab[PAR_POS_TEL5_0].uVal, testsprintf3); 
          HexTOTelephoneStr(&ParTab[PAR_POS_TEL6_0].uVal, testsprintf4);
          HexTOTelephoneStr(&ParTab[PAR_POS_TEL7_0].uVal, testsprintf5); 
          HexTOTelephoneStr(&ParTab[PAR_POS_TEL8_0].uVal, testsprintf6); 
          HexTOTelephoneStr(&ParTab[PAR_POS_TEL9_0].uVal, testsprintf7);            
          HexTOTelephoneStr(&ParTab[PAR_POS_TEL10_0].uVal, testsprintf8);            
          HexTOTelephoneStr(&ParTab[PAR_POS_TEL11_0].uVal, testsprintf9);             
          HexTOTelephoneStr(&ParTab[PAR_POS_TEL12_0].uVal, testsprintf10);
          TelephoneConnectString();
          
         at_send_data("+++", strlen("+++"),300,AT_RESP_END_OK);
         at_send_data("AT+WORKMODE=1\r\n", strlen("AT+WORKMODE=1\r\n"),300,AT_RESP_END_OK);
         at_send_data("AT+AUTOATO=0\r\n", strlen("AT+AUTOATO=0\r\n"),300,AT_RESP_END_OK);
         at_send_data((uint8_t *)pTelephoneString, strlen(pTelephoneString),500,AT_RESP_END_OK);
         at_send_data("AT&W\r\n",strlen("AT&W\r\n"),300,AT_RESP_END_OK);
         at_send_data("AT+CFUN=1,1\r\n", strlen("AT+CFUN=1,1\r\n"),500,AT_RESP_END_AT_READY);
         Send_String("你好",strlen("你好"));
          
         ParTab[PAR_POS_TEL_CONFIRM_].uVal = 0;
          
        }

    }
}



/*****************************************************************************//*!
*
* @brief   led duty control.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void LedCtr(void)
{
    sysapi->LedBreathHandle(&LedBreathObj);

    DutyIndex++;
    DutyIndex &= 0x1FF;
    DutyList[DutyIndex] = LedBreathObj.Out;

    //CPU RUN 指示
    PWMTIMER->CCR1 = DutyList[(DutyIndex - 0) & 0x1FF];
    PWMTIMER->CCR2 = DutyList[(DutyIndex - 80) & 0x1FF];

    //错误LED
    if (SysRunSt.err_global)
    {
        if ((DutyIndex & 0x3F) < 0x1F)
        {
            LED_ERROR = LED_ON;
        }
        else
        {
            LED_ERROR = LED_OFF;
        }
    }
    else
    {
        LED_ERROR = LED_OFF;
    }

    //connect 
    if (mbzObj.RunSt.bits.Connect)
    {
        LED_CONNE = LED_ON;
    }
    else
    {
        LED_CONNE = LED_OFF;
    }
}

/*****************************************************************************//*!
*
* @brief   led pwm init.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void led_init(void)
{

    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM2_FORCE_RESET();
    __HAL_RCC_TIM2_RELEASE_RESET();

    // 向上计数，仅溢出产生更新事件
    PWMTIMER->CR1 = (uint32_t)(0);
    PWMTIMER->CR2 = (uint32_t)(0);
    PWMTIMER->SMCR = (uint32_t)(0);

    //配置通道1为PWM输出，使能通道1影子寄存器
    PWMTIMER->CCMR1 = 0x6868;
    PWMTIMER->CCMR2 = 0x6868;

    //使能输出 通道同相脚 配置极性为正极性
    PWMTIMER->CCER = 0;

    //预分频系数 4
    PWMTIMER->PSC = 168>>1;
    //计数器
    PWMTIMER->CNT = 0;
    //周期装载值
    PWMTIMER->ARR = PWM_PERIOD;
    PWMTIMER->CCR1 = 0;
    PWMTIMER->CCR2 = 0;
    PWMTIMER->CCR3 = 0;
    PWMTIMER->CCR4 = 0;
    //产生一次软件更新事件
    PWMTIMER->EGR = TIM_EGR_UG; //update
    //清除更新中断标志
    PWMTIMER->SR = 0;
    //更新事件产生中断
    PWMTIMER->DIER = TIM_DIER_UIE | TIM_DIER_CC1IE | TIM_DIER_CC2IE;

    //允许中断
    NVIC_SetPriority(TIM2_IRQn, 0x04);
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(TIM2_IRQn);

    //启动定时器
    PWMTIMER->CR1 |= TIM_CR1_CEN;
}

/*****************************************************************************//*!
*
* @brief   led pwm isr.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void TIM2_IRQHandler(void)
{
    uint16_t TimerSt;

    TimerSt = PWMTIMER->SR;
    //清除中断标志
    PWMTIMER->SR = 0;
  
    if (TimerSt & TIM_SR_UIF)
    {

        LED_CPU_RUN = 0;
    }

    if (TimerSt & TIM_SR_CC1IF)
    {

    }
    if (TimerSt & TIM_SR_CC2IF)
    {
        LED_CPU_RUN = 1;
    }
}





