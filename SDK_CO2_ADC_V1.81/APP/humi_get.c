#include "osObjects.h"
#include "mbapp.h"
#include "systask.h"
#include "templib.h"
#include "partab.h"
#include "iichal.h"
#include "sys.h"
#include "max31865.h"

int32_t DataFlt(float f_real, int32_t i_now);
uint8_t SHT85_READ_BUFF[32];
uint8_t Nw_SHT85_Read(uint8_t *s) ;

/*****************************************************************************//*!
*
* @brief   humi.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void HumiGetTask(void const *argument)
{
    osDelay(3000);
    
    for(;;)
    {
        osDelay(200);
        if(IIC_ACK ==Nw_SHT85_Read(SHT85_READ_BUFF))
        {
            //大端数据格式
          
            //湿度计算
            SysRunSt.Io.SHT85_fHumi = 1000.0f * ((SHT85_READ_BUFF[4] + SHT85_READ_BUFF[3]*256) / 65535.0f);
            SysRunSt.Io.fHumi = SysRunSt.Io.fHumi * 0.8 + SysRunSt.Io.SHT85_fHumi*0.2;
            
            //湿度显示值包含一位小数点
            SysRunSt.HumiDisp = DataFlt(SysRunSt.Io.fHumi,SysRunSt.HumiDisp);
            
            SysRunSt.Io.SHT85_fTemp = -450.0f + 1750.0f * ((SHT85_READ_BUFF[1] + SHT85_READ_BUFF[0]*256) / 65535.0f);
        }
    }
}

