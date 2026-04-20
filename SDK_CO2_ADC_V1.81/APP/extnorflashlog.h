/*--------------------------------------------------------------*                                       
 *--------------------------------------------------------------*/

#ifndef _EXT_NOT_LOG_H_
#define _EXT_NOT_LOG_H_

#include <stdint.h>

#define FLASH_NOR_ERR_NONE       0
#define FLASH_NOR_ERR_SECSIZE    1  /*扇区尺寸错误*/
#define FLASH_NOR_ERR_NORSIZE    2  /*NOR数量错误*/
#define FLASH_NOR_ERR_BLKSIZE    3  /*数据块空间*/
#define FLASH_NOR_ERR_DATSIZE    4  /*实际数据个数*/
#define FLASH_NOR_ERR_NORBASE    5  /*基址错误,基址未对齐到扇区边界*/
#define FLASH_NOR_ERR_DATOVE     6  /*数据尺寸大于eep尺寸*/
#define FLASH_NOR_ERR_DATOVK     7  /*数据尺寸大于数据块*/
#define FLASH_NOR_ERR_BLKOVE     8  /*数据块大于NOR空间*/
#define FLASH_NOR_ERR_DATPER     9  /*数据尺寸必须为4的倍数*/
#define FLASH_NOR_ERR_EMPTY     10  /*扇区全空 ,表示从未写过数据*/
#define FLASH_NOR_ERR_DATBL     11  /*数据块太小，至少为4*/
#define FLASH_NOR_ERR_NORSST    12  /*NOR尺寸不是扇区的倍数*/

typedef struct{
    uint32_t EepAddr; /*当前数据起始地址*/
    uint32_t Site;     /*当前指向的位置*/
    uint32_t Blocks;   /*分割块数*/
    uint32_t AddrDataStart;   /*开始扇区地址*/
    uint32_t SiteDataStart;   /*开始扇区位号*/
    uint16_t Err;      /*上次错误代码*/
    uint16_t Checked;  /*参数已经通过检测*/
} ext_nor_log_Info_Type;

typedef const struct {
    uint32_t SectorSize; /*扇区擦除尺寸 ,2^n边界对齐  必须和硬件物理参数一致,字节数 */
    uint32_t EepSize;    /*虚拟Eep尺寸  ,扇区尺寸的2^n倍数  字节数*/
    uint32_t DataBlockSize;  /*每组数据块占用空间 ,2^边界对齐 ,字节数*/
    uint32_t DataSize;   /*实际数据块尺寸,必须为4的倍数  字节数*/
    uint32_t EepBase;    /*虚拟Eep基址  ,扇区边界对齐*/
    uint8_t *Data;       /*数据源，实际尺寸应大于等于DataSize*/
	uint32_t *WorkBuff;  /*工作缓存,不小于山区尺寸*/
    ext_nor_log_Info_Type *Info; /*运行时信息*/
    uint8_t (*Erase)(uint32_t AddrBase, uint32_t Nums); /*从指定地址擦除，Nums指定擦除的扇区个数*/
    uint8_t (*Program)(uint32_t Addr, uint32_t Len, uint8_t *Data);    /*向指定指定地址编程数据*/
    uint8_t (*Read)(uint32_t Addr, uint32_t Len, uint8_t *Data);       /*从指定位置读取数据*/
} ext_nor_log_Cfg_Type;

uint16_t ext_nor_log_FlashCheck(ext_nor_log_Cfg_Type *EepCfg);
uint16_t ext_nor_log_FlashSave(ext_nor_log_Cfg_Type *EepCfg);
uint16_t ext_nor_log_SectorAlign(ext_nor_log_Cfg_Type *EepCfg);

#endif
