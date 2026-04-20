#include "osObjects.h"
#include "mbapp.h"
#include "systask.h"
#include "templib.h"
#include "partab.h"
#include "iichal.h"
#include "sys.h"
#include "max31865.h"
#include "ADCDATA.h"


#define TEMP_ADC_ENABLE 1
#define RTD_MODE        0           //0-pt100  1-pt1000
#define RTC_RES_REF     4000//4300.0f     //参考电阻
#define RTD_LIST_SIZE   9


uint16_t rtd_val_list[3][12]; //读取传感器数据列表
uint16_t rtd_val_index[3] = { 0 }; //列表当前索引
float temp_ctr[3];
float temp_real[3];
uint32_t RtdAdSum[3] = { 0 };
uint32_t RtdCnt[3] = { 0 };
int16_t correct_pb[3];
int16_t correct_pk[3];

int32_t DataFlt(float f_real, int32_t i_now);
float Pt_ResToTemp(float m_Res);
void temp_AlSpiInit(void);

void rtd_get(uint8_t ch);
void temp_cs_sel(uint8_t cs_sel);

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
void SenModuleTask(void const *argument)
{
    osDelay(800);
    temp_AlSpiInit();  
    for (;;)
    {

        
        //修正值更新
//        correct_pb[0] = ParTab[PAR_POS_TEMP_PB].iVal;
//        correct_pk[0] = ParTab[PAR_POS_TEMP_PK].iVal;
        correct_pb[0] = ParTab[PAR_POS_DOOR_PB].iVal;   //门温度
        correct_pk[0] = ParTab[PAR_POS_DOOR_PK].iVal;
        correct_pb[2] = ParTab[PAR_POS_TEMP_PB].iVal;
        correct_pk[2] = ParTab[PAR_POS_TEMP_PK].iVal;
        
        //读取传感器
        osDelay(60);
        rtd_get(0);
        
        osDelay(10);
        rtd_get(1);
        
        osDelay(10);
        rtd_get(2);
       
          //温度计算
        SysRunSt.Io.fTempMain = temp_ctr[2];
        SysRunSt.Io.fTempDoor = temp_ctr[0];
        SysRunSt.Io.fTempEvn  = temp_ctr[1];
               
        if ( SysRunSt.Io.fTempDoor >=0 ) 
          SysRunSt.Io.iTempDoor = (int16_t)(SysRunSt.Io.fTempDoor + 0.5f);
        else
          SysRunSt.Io.iTempDoor = (int16_t)(SysRunSt.Io.fTempDoor - 0.5f);
        
        if ( SysRunSt.Io.fTempEvn >=0 ) 
          SysRunSt.Io.iTempEvn = (int16_t)(SysRunSt.Io.fTempEvn + 0.5f);
        else
          SysRunSt.Io.iTempEvn = (int16_t)(SysRunSt.Io.fTempEvn - 0.5f);
        
        SysRunSt.TempDoorDisp = DataFlt(SysRunSt.Io.fTempDoor, SysRunSt.TempDoorDisp);//滤波
        SysRunSt.TempEvnDisp = DataFlt(SysRunSt.Io.fTempEvn, SysRunSt.TempEvnDisp);//滤波

        if (SysRunSt.Io.fTempMain >= 0)
            SysRunSt.Io.iTempMain = (int16_t)(SysRunSt.Io.fTempMain + 0.5f);
        else
            SysRunSt.Io.iTempMain = (int16_t)(SysRunSt.Io.fTempMain - 0.5f);

        SysRunSt.TempMainDisp = DataFlt(SysRunSt.Io.fTempMain, SysRunSt.TempMainDisp);//滤波
        
         if( ParTab[PAR_POS_TEMP_SET].uVal- SysRunSt.TempMainDisp < ParTab[PAR_POS_TEMP_Dead_ZOON].uVal &&
             ParTab[PAR_POS_TEMP_SET].uVal- SysRunSt.TempMainDisp > -1*ParTab[PAR_POS_TEMP_Dead_ZOON].uVal )
             SysRunSt.TempMainDisp = ParTab[PAR_POS_TEMP_SET].uVal; 
      
        //门温度
        SysRunSt.Io.fTempDoor = SysRunSt.Io.fTempDoor ;   //SysRunSt.Io.fTempMain;
        SysRunSt.Io.iTempDoor = SysRunSt.Io.iTempMain ;   //SysRunSt.Io.iTempMain;
        SysRunSt.TempDoorDisp = SysRunSt.TempDoorDisp ;   //SysRunSt.TempMainDisp;

    }
}

#if TEMP_ADC_ENABLE > 0

void rtd_get(uint8_t ch)
{
    uint16_t val_list[12], val_cur;
    uint16_t cnt;
    uint8_t i, j;
    float temp_cur;

    rtd_val_index[ch]++;
    
    if (rtd_val_index[ch] >= RTD_LIST_SIZE)
        rtd_val_index[ch] = 0;

//    temp_cs_sel(ch); 
 //   rtd_val_list[ch][rtd_val_index[ch]] = max31865_rtd_read();
      rtd_val_list[ch][rtd_val_index[ch]] = ADC_Value[2+ch];
    //copy
    cnt = RTD_LIST_SIZE;
    while (cnt--)
    {
      val_list[cnt] = rtd_val_list[ch][cnt];
    }
    
    //排序
    for (j = 0; j < (RTD_LIST_SIZE - 1); j++)
    {
        for (i = 0; i < (RTD_LIST_SIZE - 1 - j); i++)
        {
            if (val_list[i] > val_list[i + 1])
            {
                uint16_t temp;

                temp = val_list[i];
                val_list[i] = val_list[i + 1];
                val_list[i + 1] = temp;
            }
        }
    }
    //取中值
    val_cur = val_list[RTD_LIST_SIZE / 2];

    RtdAdSum[ch] += val_cur;
    RtdCnt[ch]++;

    if (RtdCnt[ch] >= 8)
    {
 //       float res_pt;
        
        RtdCnt[ch] = 0;
        val_cur = RtdAdSum[ch] >> 3;
        RtdAdSum[ch] = 0;
      
        //计算阻值
//        res_pt  = (float)val_cur * RTC_RES_REF;
//        res_pt /= 1UL<<15;
      
        //计算温度

//        temp_cur = Pt_ResToTemp(res_pt);     
          temp_cur = 0.0614*val_cur-0.4161;

        if (SysRunSt.PowerUpTimeCnt > 30)
        {
            //一介滤波 //温度值放大10倍//取1位定点小数//滤波系数为两位定点小数
            temp_real[ch] = 0.01*ParTab[PAR_POS_MAIN_FILTER].uVal * temp_real[ch] + 0.01*(100 - ParTab[PAR_POS_MAIN_FILTER].uVal)*temp_cur;
//            temp_cur=  temp_real[ch] *( 1+ (float)correct_pk[ch] *0.0001 );  //将该行注释掉
//            temp_ctr[ch] = temp_real[ch] * 10.0f + (float)correct_pb[ch];  //该行修改为下面一行
            temp_ctr[ch] = temp_real[ch] * 10.0f  * (1 + (float)correct_pk[ch] *0.0001 )+ (float)correct_pb[ch] ; 

        }
        else
        {
            temp_real[ch] = temp_cur;
            temp_ctr[ch] = temp_real[ch] * 10.0f;
        }
    }

//    //启动一次转换 //三线制 //50HZ滤波
//    max31865_Cfg(0xA3);
}


#else
/*****************************************************************************//*!
* @brief   rtd temp get.
*
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

      void rtd_get(uint8_t ch)
      {

          uint16_t val_list[12], val_cur;
          uint16_t cnt;
          uint8_t i, j;
          float temp_cur;

          rtd_val_index[ch]++;
          
          if (rtd_val_index[ch] >= RTD_LIST_SIZE)
              rtd_val_index[ch] = 0;

          temp_cs_sel(ch);
          
          rtd_val_list[ch][rtd_val_index[ch]] = max31865_rtd_read();

          //copy
          cnt = RTD_LIST_SIZE;
          while (cnt--)
          {
              val_list[cnt] = rtd_val_list[ch][cnt];
          }
          
          //排序
          for (j = 0; j < (RTD_LIST_SIZE - 1); j++)
          {
              for (i = 0; i < (RTD_LIST_SIZE - 1 - j); i++)
              {
                  if (val_list[i] > val_list[i + 1])
                  {
                      uint16_t temp;

                      temp = val_list[i];
                      val_list[i] = val_list[i + 1];
                      val_list[i + 1] = temp;
                  }
              }
          }
          //取中值
          val_cur = val_list[RTD_LIST_SIZE / 2];

          RtdAdSum[ch] += val_cur;
          RtdCnt[ch]++;

          if (RtdCnt[ch] >= 8)
          {
              float res_pt;
              
              RtdCnt[ch] = 0;
              val_cur = RtdAdSum[ch] >> 3;
              RtdAdSum[ch] = 0;

              //计算阻值
              res_pt  = (float)val_cur * RTC_RES_REF;
              res_pt /= 1UL<<15;
              
              //计算温度
      #if RTD_MODE > 0
              temp_cur = Pt_ResToTemp(res_pt/10.0f);
      #else
              temp_cur = Pt_ResToTemp(res_pt);     
      #endif

              if (SysRunSt.PowerUpTimeCnt > 50)
              {
                  //一介滤波 //温度值放大10倍//取1位定点小数//滤波系数为两位定点小数
                  temp_real[ch] = 0.01*ParTab[PAR_POS_MAIN_FILTER].uVal * temp_real[ch] + 0.01*(100 - ParTab[PAR_POS_MAIN_FILTER].uVal)*temp_cur;
      //            temp_cur=  temp_real[ch] *( 1+ (float)correct_pk[ch] *0.0001 );  //将该行注释掉
      //            temp_ctr[ch] = temp_real[ch] * 10.0f + (float)correct_pb[ch];  //该行修改为下面一行
                  temp_ctr[ch] = temp_real[ch] * 10.0f  * (1 + (float)correct_pk[ch] *0.0001 )+ (float)correct_pb[ch] ; 

              }
              else
              {
                  temp_real[ch] = temp_cur;
                  temp_ctr[ch] = temp_real[ch] * 10.0f;
              }
          }

          //启动一次转换 //三线制 //50HZ滤波
          max31865_Cfg(0xA3);
      }

#endif


float Pt_ResToTemp(float m_Res)
{
    float A = 3.9083e-3;
    float B = -5.775e-7;
    float C = -4.183e-12;

    float fT, fR, fT0;
    short i;

    fT = 0;
    fR = m_Res;
    fT0 = (fR / 100 - 1) / A;
    if (fR < 18.52f)
    {
        fT = -9.0E9;
    }
    else if (fR < 100.0f)
    {
        float Error;

        for (i = 0; i < 50; i++)
        {
            fT = fT0 + (fR - 100 * (1 + A * fT0 + B * fT0 * fT0 - 100 * C * fT0 * fT0 * fT0 + C * fT0 * fT0 * fT0 * fT0)) / (100 * (A + 2 * B * fT0 - 300 * C * fT0 * fT0 + 4 * C * fT0 * fT0 * fT0));
            Error = fT - fT0;
            if ((Error < 0.001f) && (Error > -0.001f))
                break;
            else
                fT0 = fT;
        }
    }
    else if (fR < 390.482f)
    {
        float Error;

        for (i = 0; i < 50; i++)
        {
            fT = fT0 + (fR - 100 * (1 + A * fT0 + B * fT0 * fT0)) / (100 * (A + 2 * B * fT0));
            Error = fT - fT0;
            if ((Error < 0.001f) && (Error > -0.001f))
                break;
            else
                fT0 = fT;
        }
    }
    else
    {
        fT = 9.0E9;
    }

    return fT;
}

