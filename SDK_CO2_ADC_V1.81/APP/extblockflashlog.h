/*--------------------------------------------------------------*                                       
 *--------------------------------------------------------------*/

#ifndef _EXT_BLOCK_LOG_H_
#define _EXT_BLOCK_LOG_H_

#include <stdint.h>

#define FLASH_BLOCK_ERR_NONE         0
#define FLASH_BLOCK_ERR_PAGE_SIZE    1  /*扇区尺寸错误*/
#define FLASH_BLOCK_ERR_BLOCK_SIZE   2  /*BLOCK尺寸错误*/
#define FLASH_BLOCK_ERR_BLOCK_NUMS   3
#define FLASH_BLOCK_ERR_SIZE         4  /*实际数据个数*/
#define FLASH_BLOCK_ERR_PAG_PER_BLK  5  /*基址错误,基址未对齐到扇区边界*/
#define FLASH_BLOCK_ERR_LOG_SIZE     6  /*数据尺寸大于eep尺寸*/


typedef struct{
    uint32_t block_cur; /*当前数据起始地址*/
    uint32_t block_start;     /*当前指向的位置*/
    uint16_t Err;      /*上次错误代码*/
    uint8_t Checked;  /*参数已经通过检测*/
	uint8_t full; /*数据已经存满，继续存储将开始覆盖式写入*/
} ext_block_log_Info_Type;


typedef const struct {
	uint32_t block_base;    /*起始块基址*/
	uint32_t block_amount;
    uint32_t page_size; /*扇区擦除尺寸 ,2^n边界对齐  必须和硬件物理参数一致,字节数 */
    uint32_t block_size;    /*块尺寸  ,扇区尺寸的2^n倍数  字节数*/
	uint32_t size; /*总尺寸，字节数*/
    uint32_t page_amount_per_block; /*每块页数*/
	uint32_t log_block_size;/*每条日志尺寸，用于搜索位置*/
    uint8_t *Data;       /*数据源，实际尺寸应大于等于DataSize*/
	uint32_t *WorkBuff;  /*工作缓存,不小于日志尺寸*/
    ext_block_log_Info_Type *Info; /*运行时信息*/
    uint8_t (*Erase)(uint32_t block_addr, uint32_t Nums); /*从指定地址擦除，Nums指定擦除的扇区个数*/
    uint8_t (*Program)(uint32_t block_addr, uint8_t *Data);    /*向指定指定地址编程数据*/
    uint8_t (*Read)(uint32_t page_ddr, uint32_t Len, uint8_t *Data);       /*从指定位置读取数据*/
} ext_block_log_Cfg_Type;

uint16_t ext_block_log_FlashCheck(ext_block_log_Cfg_Type *EepCfg);
uint16_t ext_block_log_FlashSave(ext_block_log_Cfg_Type *EepCfg);

#endif
