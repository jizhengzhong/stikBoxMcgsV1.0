/* co2浓度获取
*
*  传感器配置为modbus模式，波特率19200，站号240，无校验，2停止位
*/

#include "osObjects.h"
#include "mbapp.h"
#include "systask.h"
#include "templib.h"
#include "partab.h"
#include "iichal.h"
#include "sys.h"
#include "max31865.h"
#include "adcdata.h"

#define N 21
#define CO2_Relay_Switch  0
//#define CO2_RS485   1

extern timer_pulse_type CtrPls;
int32_t DataFlt(float f_real, int32_t i_now);

uint16_t co2_buff[16];
unsigned int ADC_Value[5]={0};
unsigned int temp_adc,value_buf[N]={0};
typedef union 
{
        float fval;
        uint16_t wd[2];
} f_i_type;

f_i_type co2_col_read;

unsigned int middleValueFilter(unsigned int* adc_buf)
{
    int i,j,k,temp;
    for( i = 0; i < N; ++i)
      {
        value_buf[i] = adc_buf[0];
        osDelay(2);
      }
   
    for(j = 0 ; j < N-1; ++j)
    {
        for(k = 0; k < N-j-1; ++k)
        {
            //从小到大排序，冒泡法排序
            if(value_buf[k] > value_buf[k+1])
            {
                temp = value_buf[k];
                value_buf[k] = value_buf[k+1];
                value_buf[k+1] = temp;
            }
        }
    }
		
    return value_buf[(N-1)/2];
}

/*****************************************************************************//*!
*
* @brief   co2.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

#ifndef CO2_RS485

void Co2GetTask(void const *argument)
{
   short int Pb,Pk;
   static float CO2_Temp;

   static unsigned short Cal_count=0;

    HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&ADC_Value,5);    //使用DMA传输
    osDelay(1000);
      SysRunSt.LstColDis = (int16_t)(110.0*ADC_Value[1]/2048)*10;
//      Pressure = 3.3*ADC_Value[0]/4095;
     SysRunSt.Io.OutPut.bits.out_WATER_PUMP=0; //co2切换阀1
    //GMP251 寄存器地址见 附录A.2?Modbus Registers
                    SysRunSt.Io.OutPut.bits.out_WATER_PUMP = 0;
               SysRunSt.Io.OutPut.bits.out_COOL_PUMP   = 0;

    for(;;)
    {
        osThreadFlagsWait(MBAPP_EVENT_MSK,osFlagsWaitAny, 500);
        osDelay(10);
        temp_adc = middleValueFilter((unsigned int*)&ADC_Value[1]);
        CO2_Temp = 3.3*temp_adc/4096.0/0.6;
        CO2_Temp = CO2_Temp*10 - ParTab[PAR_POS_CO_VL].iVal;
        CO2_Temp = CO2_Temp/( ParTab[PAR_POS_CO_VH].iVal-ParTab[PAR_POS_CO_VL].iVal );
        CO2_Temp = CO2_Temp*200;
//        co2_col_read.fval = 110.0*temp_adc/2048; //换算为1位小数
//        temp_adc = middleValueFilter((unsigned int*)&ADC_Value[0]);
//        Pressure = 3.3*temp_adc/4095;
        Pk=(short int)ParTab[PAR_POS_COL_PK].iVal;
        Pb=(short int)ParTab[PAR_POS_COL_PB].iVal;
        co2_col_read.fval = CO2_Temp + CO2_Temp * Pk / 10000;
        SysRunSt.Io.fCol = co2_col_read.fval + Pb/10.0; //换算为1位小数
//        co2_col_read.fval = co2_col_read.fval + co2_col_read.fval * (float)ParTab[PAR_POS_COL_PK].iVal / 10000;
//        SysRunSt.Io.fCol = co2_col_read.fval + ((float)ParTab[PAR_POS_COL_PB].iVal)/10; //换算为1位小数
        SysRunSt.Io.iCol = DataFlt(SysRunSt.Io.fCol,SysRunSt.Io.iCol);
        SysRunSt.ColDisp =(int16_t)(0.9*DataFlt(SysRunSt.Io.fCol*10,SysRunSt.ColDisp) + 0.1*SysRunSt.LstColDis ); //显示为2位小数
         if( ParTab[PAR_POS_COL_SET].uVal*10- SysRunSt.ColDisp < 10 &&
             ParTab[PAR_POS_COL_SET].uVal*10- SysRunSt.ColDisp > -10 )
             SysRunSt.ColDisp = ParTab[PAR_POS_COL_SET].uVal*10;
        SysRunSt.LstColDis = SysRunSt.ColDisp;
        
    if(CO2_Relay_Switch == 1)  
      {
        if( CtrPls.PulseTimeCnt_1s )
          {
            if( ADC_Value[0] < 1050 )
              Cal_count++;
            else
              Cal_count=0;
          } 
          if( Cal_count > 16)
          {
             if( SysRunSt.Io.OutPut.bits.out_WATER_PUMP == 1)
             {
               Cal_count = 0;
               SysRunSt.Io.OutPut.bits.out_WATER_PUMP = 0;
               SysRunSt.Io.OutPut.bits.out_COOL_PUMP   = 1;
             } else
             {
               Cal_count = 0;
               SysRunSt.Io.OutPut.bits.out_WATER_PUMP = 1;
               SysRunSt.Io.OutPut.bits.out_COOL_PUMP   = 0;
             }
          
          }
      }else
      {
      
      }
    }

}

//中值滤波算法

#else
void Co2GetTask(void const *argument)
{
 //  unsigned int ADC_Value[2]={0};
    osDelay(1000);
    //co2 端口
    mbzInit(MB_RTU_MASTER, 19200, MB_PAR_NONE);
    mbzObj.SlaveId=240;


    //GMP251 寄存器地址见 附录A.2?Modbus Registers
    
    
    sysapi->mbStaticCmdSetNoOS(&mbzObj, 0, 240, FUN_CODE_READ_REG, co2_buff, 0, 8, 0);
    
//    sysapi->mbStaticCmdSetNoOS(&mbzObj, 1, 240, FUN_CODE_READ_REG, &co2_buff[8], 0x100, 2, 0);
 
    for(;;)
    {
        osThreadFlagsWait(MBAPP_EVENT_MSK,osFlagsWaitAny, 500);
        osDelay(10);  
        sysapi->mb_poll (&mbzObj);
        
        //CO2浓度 ppm
        co2_col_read.wd[0] = co2_buff[0];
        co2_col_read.wd[1] = co2_buff[1];  
        co2_col_read.fval /=1000; //保留1位小数
        
        co2_col_read.fval = co2_col_read.fval + co2_col_read.fval * (float)ParTab[PAR_POS_COL_PK].iVal / 10000;
        
        SysRunSt.Io.fCol = co2_col_read.fval + ((float)ParTab[PAR_POS_COL_PB].iVal)/10; //换算为1位小数
        SysRunSt.Io.iCol = DataFlt(SysRunSt.Io.fCol,SysRunSt.Io.iCol);
        SysRunSt.ColDisp = DataFlt(SysRunSt.Io.fCol*10,SysRunSt.ColDisp); //显示为2位小数
    }

}
#endif
