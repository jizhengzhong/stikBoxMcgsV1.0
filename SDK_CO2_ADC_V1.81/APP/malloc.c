#include "malloc.h"	   
#include "osObjects.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//modify 1st: apleilx @ 2017/11/25
//All rights reserved 
////////////////////////////////////////////////////////////////////////////////// 	 


//内存池(32字节对齐)
static u8  mem1_base[MEM1_MAX_SIZE]; //内部SRAM内存池
static u8  mem3_base[MEM3_MAX_SIZE]	 @ ".ccmram_alig";	 //内部CCM内存池//必须在ICF文件中添加CCMRAM的段定义

#define mem2_base ((u8*)(MEM2_BASE))	           			                        //外部SDRAM内存池,前面2M给LTDC用了(1280*800*2)

//内存管理表
static u32 mem1_table_base[MEM1_ALLOC_TABLE_SIZE]; //内部SRAM内存池table
#define mem2_table_base ((u32*)(MEM2_BASE + MEM2_MAX_SIZE))                         //外部SRAM内存池table  //存储区顶部作为分配表
static u32 mem3_table_base[MEM3_ALLOC_TABLE_SIZE] @ ".ccmram_alig";	                    //内部CCM内存池table   //存储区顶部作为分配表

//内存管理参数	   
const u32 mem_table_size[SRAM_BANKS] = { MEM1_ALLOC_TABLE_SIZE, MEM2_ALLOC_TABLE_SIZE, MEM3_ALLOC_TABLE_SIZE }; //内存表大小
const u32 mem_block_size[SRAM_BANKS] = { MEM1_BLOCK_SIZE, MEM2_BLOCK_SIZE, MEM3_BLOCK_SIZE }; //内存分块大小
const u32 mem_size[SRAM_BANKS] = { MEM1_MAX_SIZE, MEM2_MAX_SIZE, MEM3_MAX_SIZE }; //内存总大小

static u8 my_mem_man_rdy[SRAM_BANKS] = { 0, 0, 0 }; //内存管理是否就绪

//内存管理控制器
const mallco_dev_type mallco_dev = {
    my_mem_init, //内存初始化
    my_mem_perused, //内存使用率
    mem1_base, mem2_base, mem3_base, //内存池
    mem1_table_base, mem2_table_base, mem3_table_base, //内存管理状态表
};


//复制内存
//*des:目的地址
//*src:源地址
//n:需要复制的内存长度(字节为单位)

void mymemcpy(void *des, void *src, u32 n) {
    u8 *xdes = des;
    u8 *xsrc = src;
    while (n--)
        *xdes++ = *xsrc++;
}
//设置内存
//*s:内存首地址
//c :要设置的值
//count:需要设置的内存大小(字节为单位)

void mymemset(void *s, u8 c, u32 count) {
    u8 *xs = s;
    while (count--)
        *xs++ = c;
}
//内存管理初始化  
//memx:所属内存块

void my_mem_init(u8 memx) {
    mymemset(mallco_dev.mem_table[memx], 0, mem_table_size[memx] * 4); //内存状态表数据清零  
    my_mem_man_rdy[memx] = 1; //内存管理初始化OK  
}
//获取内存使用率
//memx:所属内存块
//返回值:使用率(扩大了10倍,0~1000,代表0.0%~100.0%)

u16 my_mem_perused(u8 memx) {
    u32 used = 0;
    u32 i;
    for (i = 0; i < mem_table_size[memx]; i++) {
        if (mallco_dev.mem_table[memx][i])used++;
    }
    return (used * 1000) / (mem_table_size[memx]);
}
//内存分配(内部调用)
//memx:所属内存块
//size:要分配的内存大小(字节)
//返回值:0XFFFFFFFF,代表错误;其他,内存偏移地址 

u32 my_mem_malloc(u8 memx, u32 size) {
    signed long offset = 0;
    u32 nmemb; //需要的内存块数  
    u32 cmemb = 0; //连续空内存块数
    u32 i;
    if (!my_mem_man_rdy[memx])
        mallco_dev.init(memx); //未初始化,先执行初始化 
    if (size == 0)
        return 0XFFFFFFFF; //不需要分配
    nmemb = size / mem_block_size[memx]; //获取需要分配的连续内存块数
    if (size % mem_block_size[memx])
        nmemb++;
    for (offset = mem_table_size[memx] - 1; offset >= 0; offset--)//搜索整个内存控制区  
    {
        if (!mallco_dev.mem_table[memx][offset])
            cmemb++; //连续空内存块数增加
        else
            cmemb = 0; //连续内存块清零
        if (cmemb == nmemb) //找到了连续nmemb个空内存块
        {
            for (i = 0; i < nmemb; i++) //标注内存块非空 
            {
                mallco_dev.mem_table[memx][offset + i] = nmemb;
            }
            return (offset * mem_block_size[memx]); //返回偏移地址  
        }
    }
    return 0XFFFFFFFF; //未找到符合分配条件的内存块  
}
//释放内存(内部调用) 
//memx:所属内存块
//offset:内存地址偏移
//返回值:0,释放成功;1,释放失败;  

u8 my_mem_free(u8 memx, u32 offset) {
    int i;
    if (!my_mem_man_rdy[memx])//未初始化,先执行初始化
    {
        mallco_dev.init(memx);
        return 1; //未初始化  
    }
    if (offset < mem_size[memx])//偏移在内存池内. 
    {
        int index = offset / mem_block_size[memx]; //偏移所在内存块号码  
        int nmemb = mallco_dev.mem_table[memx][index]; //内存块数量
        for (i = 0; i < nmemb; i++) //内存块清零
        {
            mallco_dev.mem_table[memx][index + i] = 0;
        }
        return 0;
    } else return 2; //偏移超区了.  
}
//释放内存(外部调用) 
//memx:所属内存块
//ptr:内存首地址 

void myfree(u8 memx, void *ptr) {
    u32 offset;

    osMutexWait(mid_malloc, 1000);

    if (ptr != NULL){ //地址为0.  
        offset = (u32)ptr - (u32)mallco_dev.mem_base[memx];
        my_mem_free(memx, offset); //释放内存    
    }

    osMutexRelease(mid_malloc);
}
//分配内存(外部调用)
//memx:所属内存块
//size:内存大小(字节)
//返回值:分配到的内存首地址.

void *mymalloc(u8 memx, u32 size) {
    u32 offset;
    void *pDes;

    osMutexWait(mid_malloc, 1000);

    offset = my_mem_malloc(memx, size);
    if (offset == 0XFFFFFFFF)
        pDes = NULL;
    else
        pDes = (void*)((u32)mallco_dev.mem_base[memx] + offset);

    osMutexRelease(mid_malloc);

    return pDes;
}
//重新分配内存(外部调用)
//memx:所属内存块
//*ptr:旧内存首地址
//size:要分配的内存大小(字节)
//返回值:新分配到的内存首地址.

void *myrealloc(u8 memx, void *ptr, u32 size) {
    u32 offset;
    offset = my_mem_malloc(memx, size);
    if (offset == 0XFFFFFFFF)return NULL;
    else {
        mymemcpy((void*)((u32)mallco_dev.mem_base[memx] + offset), ptr, size); //拷贝旧内存内容到新内存   
        myfree(memx, ptr); //释放旧内存
        return (void*)((u32)mallco_dev.mem_base[memx] + offset); //返回新内存首地址
    }
}












