#define _STR_MODULE_

#include "osObjects.h"
#include "strctr.h"
#include "pincfg.h"
#include "systask.h"
#include "malloc.h"
#include "usb_host.h"
#include "fatfs.h"
#include "ff.h"

extern USBH_HandleTypeDef hUsbHostFS;

FATFS FatFs[2]; /* File system object for logical drive */
FIL fil[2];
FRESULT FsSt,FatSt;
DIR fildir;
uint32_t Len;
uint32_t Lenw;
    
uint8_t bSdTest=0;
uint8_t DataBuff[4100];

void UsbMassInit(void);
void FillRwTest(void);
void FlashQ25CmdHandle(void);

void UsbFoundHandle(void);
void Sd_fil_test(void);

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

void StrTask(void const *argument) {
    uint32_t UsbFoundTimeCnt;
    osDelay(1000);
    
    USBH_DeInit(&hUsbHostFS);
    
    MX_USB_HOST_Init();
    
    for (;;) {
 
        MX_USB_HOST_Process();
        
        //U盘读写测试
        if ( HOST_CLASS == hUsbHostFS.gState ) {
            if(UsbFoundTimeCnt == 40){
                FillRwTest();
            } else if( UsbFoundTimeCnt > 40){

                if(StHisDataRead.HisDataReadReq){
                    StHisDataRead.HisDataReadReq = 0;
                }
            } else {
                    
            }
        
        } else {
            UsbFoundTimeCnt = 0;
            
        }

        UsbFoundTimeCnt++;  
        
        //SD卡读写测试
        if( SysRunSt.sd_cd_st){
            if(!bSdTest)
            {
                Sd_fil_test();
            }
        }
        else
        {
            bSdTest = 0;
        }
    }
}


/*****************************************************************************//*!
*
* @brief   file read and write test.
*     
* @param   none
*
* @return  none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/

void FillRwTest(void) {
    uint32_t Cnt;
    UINT FsResdLen;

    f_mount(&FatFs[0], "0:", 1);


    if (FR_OK != FsSt) {
        f_mount(NULL, "0:", 0);
        return;
    }

    FsSt = f_open(&fil[0], "app.c", FA_READ);
    if (FsSt == FR_OK) {
       
        FsResdLen = 1024;
        FsSt = f_lseek(&fil[0], 0);

        FsSt = f_read(&fil[0], &StHisDataRead.Buff1.b[0], 4096, &FsResdLen);

        f_close(&fil[0]);
       
    } else {
    }
    
    //写入文件测试
    FsSt = f_open(&fil[0], "explor.csv", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    if (FsSt == FR_OK) {
        FsSt = f_lseek(&fil[0], f_size(&fil[0]));
    }
    if (FsSt == FR_OK) {
        Cnt = 16;
        while(Cnt--){
            sprintf((char*)&StHisDataRead.Buff1.b[Cnt*64],"000001,000002,000003,000004,000005,000006,000007,000008,000009\r\n");
        }
        
        Cnt = 128;
        do {
            if (FR_OK != FsSt) break;
            FsSt = f_write(&fil[0], StHisDataRead.Buff1.b, 4096, &FsResdLen);

            if (FR_OK != FsSt) break;
            FsSt = f_write(&fil[0], StHisDataRead.Buff1.b, 4096, &FsResdLen);
        } while (Cnt--);
    }

    f_close(&fil[0]);

    f_mount(NULL, "0:", 0);
}

void Sd_fil_test(void) {

    uint32_t Cnt;
    FIL * pFile;
    FATFS * fs;
    
    fs = &FatFs[1];
    pFile = &fil[1];
    
    FatSt = f_mount(fs, "1:", 1);

    if (FR_OK == FatSt) {
        do {
            //打开文件
            FatSt = f_open(pFile, "1:app.c", FA_READ);
            if (FR_OK != FatSt) break;


            FatSt = f_lseek(pFile, 0);
            if (FR_OK != FatSt) break;  
            
            FatSt = f_read(pFile, DataBuff, 2048, &Len);
            if (FR_OK != FatSt) break;  
            
            if (FR_OK != FatSt) break;   
            
            FatSt = f_close(pFile);
            if (FR_OK != FatSt) break;

            //打开文件
            FatSt = f_open(pFile, "1:export.txt", FA_WRITE | FA_READ | FA_OPEN_ALWAYS);
            if (FR_OK != FatSt) break;

            FatSt = f_lseek(pFile, f_size(pFile));
            if (FR_OK != FatSt) break;

            Lenw = Len;
            for (Cnt = 0; Cnt < 16; Cnt++) {
                FatSt = f_write(pFile, DataBuff, Lenw, &Len);
                if (FR_OK != FatSt) break;
            }
            
            FatSt = f_close(pFile);
            
            bSdTest = 1;
            break;

        } while (1);
        //关闭文件

    }

    

    //卸载文件系统
    FatSt = f_mount(NULL, "1:", 1);
}

