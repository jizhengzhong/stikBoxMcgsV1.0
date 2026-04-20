/*--------------------------------------------------------------*                                       
 *--------------------------------------------------------------*/

#ifndef _FlashToEEP_H_
#define _FlashToEEP_H_

#include <stdint.h>

#define FLASH_EEP_ERR_NONE 0
#define FLASH_EEP_ERR_SECSIZE 1 /*扇区尺寸错误*/
#define FLASH_EEP_ERR_EEPSIZE 2 /*EEP数量错误*/
#define FLASH_EEP_ERR_BLKSIZE 3 /*数据块空间*/
#define FLASH_EEP_ERR_DATSIZE 4 /*实际数据个数*/
#define FLASH_EEP_ERR_EEPBASE 5 /*基址错误,基址未对齐到扇区边界*/
#define FLASH_EEP_ERR_DATOVE 6  /*数据尺寸大于eep尺寸*/
#define FLASH_EEP_ERR_DATOVK 7  /*数据尺寸大于数据块*/
#define FLASH_EEP_ERR_BLKOVE 8  /*数据块大于EEP空间*/
#define FLASH_EEP_ERR_DATPER 9  /*数据尺寸必须为4的倍数*/
#define FLASH_EEP_ERR_EMPTY 10  /*扇区全空 ,表示从未写过数据*/
#define FLASH_EEP_ERR_DATBL 11  /*数据块太小，至少为4*/
#define FLASH_EEP_ERR_EEPSST 12 /*EEP尺寸不是扇区的倍数*/
#define FLASH_EEP_ERR_EXTPAG 13 /*EEP尺寸不是扇区的倍数*/

/*--------------------------------------------Inner Flash to eep-----------------------------------------------*/
typedef struct
{
    uint32_t EepAddr;       /*当前数据起始地址*/
    uint32_t Site;          /*当前指向的位置*/
    uint32_t Blocks;        /*分割块数*/
    uint32_t AddrDataStart; /*开始扇区地址*/
    uint32_t SiteDataStart; /*开始扇区位号*/
    uint16_t Err;           /*上次错误代码*/
    uint16_t Checked;       /*参数已经通过检测*/
} FlashEepInfo_Type;

typedef const struct
{
    uint32_t SectorSize;                             /*扇区擦除尺寸 ,2^n边界对齐  必须和硬件物理参数一致,字节数 */
    uint32_t EepSize;                                /*虚拟Eep尺寸  ,扇区尺寸的2^n倍数  字节数*/
    uint32_t DataBlockSize;                          /*每组数据块占用空间 ,2^边界对齐 ,字节数*/
    uint32_t DataSize;                               /*实际数据块尺寸,必须为4的倍数  字节数*/
    uint32_t EepBase;                                /*虚拟Eep基址  ,扇区边界对齐*/
    uint16_t *Data;                                  /*数据源，实际尺寸应大于等于DataSize*/
    FlashEepInfo_Type *Info;                         /*运行时信息*/
    void (*Erase)(uint32_t AddrBase, uint32_t Nums); /*从指定地址擦除，Nums指定擦除的扇区个数*/
    void (*Program)(uint32_t Addr, uint32_t Data);   /*向指定指定地址编程数据*/
    void (*Unlock)(void);                            /*flash解锁  0不执行*/
    void (*Lock)(void);                              /*flash锁定  0不执行*/
} FlashEepCfg_Type;

uint16_t FlashEepRead(FlashEepCfg_Type *EepCfg);
uint16_t FlashEepSave(FlashEepCfg_Type *EepCfg);

uint16_t FlashMulEepSave(FlashEepCfg_Type *EepCfg);
uint16_t FlashMulEepRead(FlashEepCfg_Type *EepCfg);

/*--------------------------------------------ext Flash to eep-----------------------------------------------*/
typedef struct
{
    uint32_t EepAddr;       /*当前数据起始地址*/
    uint32_t Site;          /*当前指向的位置*/
    uint32_t Blocks;        /*分割块数*/
    uint32_t AddrDataStart; /*开始扇区地址*/
    uint32_t SiteDataStart; /*开始扇区位号*/
    uint16_t Err;           /*上次错误代码*/
    uint16_t Checked;       /*参数已经通过检测*/
} ExtFlashEepInfo_Type;

typedef const struct
{
    uint32_t SectorSize;    /*扇区擦除尺寸 ,2^n边界对齐  必须和硬件物理参数一致,字节数 */
    uint32_t EepSize;       /*虚拟Eep尺寸  ,扇区尺寸的2^n倍数  字节数*/
    uint32_t PageSize;      /*外部flash页尺寸2^n*/
    uint32_t DataBlockSize; /*每组数据块占用空间 ,2^边界对齐 ,字节数*/
    uint32_t DataSize;      /*实际数据块尺寸,必须为4的倍数  字节数, 最后4字节预留为校验码*/
    uint32_t EepBase;       /*虚拟Eep基址  ,扇区边界对齐*/
    uint8_t *Data;          /*数据源，实际尺寸应大于等于DataSize,前2字节不允许全为0xFF，用以确认数据是否保存程序，防止存数据过程中断电导致数据不完整*/
    uint32_t *WorkBuff;
    ExtFlashEepInfo_Type *Info;                                  /*运行时信息*/
    uint8_t (*Erase)(uint32_t AddrBase, uint32_t Nums);             /*从指定地址擦除，Nums指定擦除的扇区个数*/
    uint8_t (*Program)(uint32_t Addr, uint8_t *Data, uint16_t Len); /*向指定指定地址编程数据*/
    uint8_t (*Read)(uint32_t Addr, uint8_t *Data, uint16_t Len);
} ExtFlashEepCfg_Type;

uint16_t ExtFlashMulEepSave(ExtFlashEepCfg_Type *EepCfg);
uint16_t ExtFlashMulEepRead(ExtFlashEepCfg_Type *EepCfg);

/*--------------------------------------------Inner Flash to Log Buffer--------------------------------------*/
typedef struct
{
    uint32_t EepAddr; /*当前数据起始地址*/
    uint32_t Site;    /*指向当前待存储位置*/
    uint32_t Blocks;  /*分割块数*/
    uint16_t Err;     /*上次错误代码*/
    uint16_t Checked; /*参数已经通过检测*/
} flash_log_man_info_Type;

typedef const struct
{
    uint32_t EepBase;                                             /*虚拟Eep基址  ,扇区边界对齐*/
    uint32_t SectorSize;                                          /*扇区擦除尺寸 ,2^n边界对齐  必须和硬件物理参数一致,字节数 */
    uint32_t SectorAmount;                                        /*虚拟Eep尺寸  ,扇区尺寸的2^n倍数  扇区数量*/
    uint32_t DataBlockSize;                                       /*每组数据块占用空间 ,2^边界对齐 ,字节数*/
    uint32_t DataSize;                                            /*实际数据块尺寸,必须为4的倍数  字节数*/
    uint16_t *Data;                                               /*数据源，实际尺寸应大于等于DataSize*/
    flash_log_man_info_Type *Info;                                /*运行时信息*/
    void (*Erase)(uint32_t AddrBase, uint32_t Nums);              /*从指定地址擦除，Nums指定擦除的扇区个数*/
    void (*Program)(uint32_t Addr, uint16_t *Data, uint16_t Len); /*向指定指定地址编程数据*/
    void (*Unlock)(void);                                         /*flash解锁  0不执行*/
    void (*Lock)(void);                                           /*flash锁定  0不执行*/
    void (*FlashFullCallBack)(void);                              /*存满回调*/
} flash_log_man_cfg_Type;

uint16_t FlashLogSave(flash_log_man_cfg_Type *EepCfg);
uint16_t FlashLogInit(flash_log_man_cfg_Type *EepCfg);
uint16_t FlashLogSectorAlign(flash_log_man_cfg_Type *EepCfg);

/*--------------------------------------------ext Flash to Log Buffer--------------------------------------*/
typedef struct
{
    uint32_t EepAddr; /*当前数据起始地址*/
    uint32_t Site;    /*指向当前待存储位置*/
    uint32_t Blocks;  /*分割块数*/
    uint16_t Err;     /*上次错误代码*/
    uint16_t Checked; /*参数已经通过检测*/
} ext_flash_log_man_info_Type;

typedef const struct
{
    uint32_t EepBase;                                            /*虚拟Eep基址  ,扇区边界对齐*/
    uint32_t SectorSize;                                         /*扇区擦除尺寸 ,2^n边界对齐  必须和硬件物理参数一致,字节数 */
    uint32_t SectorAmount;                                       /*虚拟Eep尺寸  ,扇区尺寸的2^n倍数  扇区数量*/
    uint32_t DataBlockSize;                                      /*每组数据块占用空间 ,2^边界对齐 ,字节数*/
    uint32_t DataSize;                                           /*实际数据块尺寸,必须为4的倍数  字节数，此程序不检查页尺寸，数据块应小于一物理页flash*/
    uint8_t *Data;                                               /*数据源，实际尺寸应大于等于DataSize*/
    uint32_t *WorkBuff;                                          /*工作缓存*/
    ext_flash_log_man_info_Type *Info;                           /*运行时信息*/
    void (*Erase)(uint32_t AddrBase, uint32_t Nums);             /*从指定地址擦除，Nums指定擦除的扇区个数*/
    void (*Program)(uint32_t Addr, uint8_t *Data, uint16_t Len); /*向指定指定地址编程数据*/
    void (*Read)(uint32_t Addr, uint8_t *Data, uint16_t Len);
    void (*FlashFullCallBack)(void); /*存满回调*/
} ext_flash_log_man_cfg_Type;

uint16_t ExtFlashLogSave(ext_flash_log_man_cfg_Type *EepCfg);
uint16_t ExtFlashLogInit(ext_flash_log_man_cfg_Type *EepCfg);
uint16_t ExtFlashLogSectorAlign(ext_flash_log_man_cfg_Type *EepCfg);

#endif
