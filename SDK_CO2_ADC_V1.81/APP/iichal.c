/****************************************************************************
 * iichal.c
 * Tested MCU 
 * Owner : apleilx
 * **************************************************************************/
#define _IIC_HAL_MODULE_

#include "iichal.h"
#include "pincfg.h"
#include "sys.h"
#include "delay.h"
#include "osObjects.h"

#define  delayx1us(x) delay_us(x)
     
//IO方向设置
#define SDA_OUT()  {__disable_irq(); GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;__enable_irq();}
#define SDA_IN()   {__disable_irq(); GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;__enable_irq();}

//IO操作函数	 
#define IIC_SCL    PBout(6) 		//SCL
#define IIC_SDA    PBout(7) 		//SDA	 
#define READ_SDA   PBin(7) 	 		//输入SDA 

/*端口位定义*/
#define SDA_OUT1()   IIC_SDA=1
#define SDA_OUT0() IIC_SDA=0
#define SDA_ST()   READ_SDA
#define SCL_OUT1()  IIC_SCL=1
#define SCL_OUT0()  IIC_SCL=0

#define IIC_SCL_OUT()  SCL_OUT()
#define IIC_SDA_OUT()  SDA_OUT()
#define IIC_SDA_IN()   SDA_IN()

/*状态标志*/
static uint8_t Ack; /*应答标志位*/

/*********************************************************************
 ********************************************************************/
void Nw_Start_I2c() {
    IIC_SDA_OUT();
    SDA_OUT1(); /*发送起始条件的数据信号*/
    delayx1us(1);
    SCL_OUT1();
    delayx1us(5); /*起始条件建立时间大于4.7us,延时*/
    SDA_OUT0(); /*发送起始信号*/
    delayx1us(5);
    SCL_OUT0(); /*钳住I2C总线，准备发送或接收数据 */
    delayx1us(2);
}

/***********************************************
 ***********************************************/
void Nw_Stop_I2c() {
    IIC_SDA_OUT();
    SCL_OUT0();
    SDA_OUT0(); /*发送结束条件的数据信号*/
    delayx1us(1); /*发送结束条件的时钟信号*/
    SCL_OUT1(); /*结束条件建立时间大于4μs*/
    delayx1us(5);
    SDA_OUT1(); /*发送I2C总线结束信号*/
    delayx1us(5);
}

/********************************************************************
 ********************************************************************/
void Nw_Ack_I2c(uint8_t a) {
    SCL_OUT0();
    IIC_SDA_OUT();

    if (a == IIC_ACK)
        SDA_OUT0(); //在此发出应答或非应答信号
    else 
        SDA_OUT1();
    delayx1us(3);
    SCL_OUT1();
    delayx1us(5);
    SCL_OUT0(); //清时钟线，钳住I2C总线以便继续接收
    delayx1us(2);
}

/*****************************************************************
 ********************************************************************/
void Nw_SendByte(uint8_t c) {
    uint8_t BitCnt;
    IIC_SDA_OUT();
    
    SCL_OUT0();  //////////ji_ADD 
    
    for (BitCnt = 0; BitCnt < 8; BitCnt++) /*要传送的数据长度为8位*/ {
      { 
        if ((c << BitCnt) & 0x80) 
          SDA_OUT1(); /*判断发送位*/
        else 
          SDA_OUT0();
      }
        delayx1us(2);
        SCL_OUT1(); /*置时钟线为高，通知被控器开始接收数据位*/
        delayx1us(5);
        SCL_OUT0();
        delayx1us(5);
    }
    delayx1us(2);
    IIC_SDA_IN(); /*8位发送完后释放数据线，准备接收应答位*/
    SDA_OUT1();/////ji_add增加释放
    delayx1us(2);
    SCL_OUT1();
    delayx1us(200);
    if (SDA_ST()) Ack = IIC_NACK;
    else Ack = IIC_ACK; /*判断是否接收到应答信号*/
    SCL_OUT0();
    delayx1us(2);
}

/*******************************************************************
 ********************************************************************/
uint8_t Nw_ISendStr(uint8_t sla, uint8_t suba, uint8_t *s, uint8_t no) {
    uint8_t i;
	
	osMutexWait( mid_iichal, INT32_MAX );
	
    Nw_Start_I2c(); //启动总线
    Nw_SendByte(sla); //发送器件地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    Nw_SendByte(suba); //发送器件子地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    for (i = 0; i < no; i++) {
        Nw_SendByte(*s); //发送数据
        if (Ack == IIC_NACK) {
            Nw_Stop_I2c();
			osMutexRelease( mid_iichal );
            return (IIC_NACK);
        }
        s++;
    }
    Nw_Stop_I2c(); //结束总线
	
	osMutexRelease( mid_iichal );
    return (IIC_ACK);
}

/*******************************************************************
字节数据传送函数               
函数原型: uchar  RcvByte();
功能:用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
发完后请用应答函数。  
 ********************************************************************/
uint8_t Nw_RcvByte() {
    uint8_t retc;
    uint8_t BitCnt;

    retc = 0;
    IIC_SDA_IN();

    for (BitCnt = 0; BitCnt < 8; BitCnt++) {
        delayx1us(1);
        SCL_OUT0(); //置时钟线为低，准备接收数据位
        delayx1us(5);
        SCL_OUT1(); //置时钟线为高使数据线上数据有效
        delayx1us(2);
        retc = retc << 1;
        if (SDA_ST())retc = retc + 1; //读数据位,接收的数据位放入retc中
        delayx1us(2);
    }
    SCL_OUT0();
    delayx1us(2);
    return (retc);
}

/**********************************************************************
向有子地址器件读取多字节数据函数               
函数原型: bit  ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
功能:从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件地址sla，
子地址suba，读出的内容放入s指向的存储区，读no个字节。如果返回1
表示操作成功，否则操作有误。
 **********************************************************************/
uint8_t Nw_IRcvStr(uint8_t sla, uint8_t suba, uint8_t *s, uint8_t no) {
    uint8_t i;

    osMutexWait( mid_iichal, INT32_MAX );
    
    Nw_Start_I2c(); //启动总线
    Nw_SendByte(sla); //发送器件地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
        osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    Nw_SendByte(suba); //发送器件子地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    Nw_Start_I2c();
    Nw_SendByte(sla + 1);
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    for (i = 0; i < no - 1; i++) {
        *s = Nw_RcvByte(); //发送数据
        Nw_Ack_I2c(IIC_ACK); //发送就答位
        s++;
    }
    *s = Nw_RcvByte();
    Nw_Ack_I2c(IIC_NACK); //发送非应位
    Nw_Stop_I2c(); //结束总线
	
	osMutexRelease( mid_iichal );
    return (IIC_ACK);
}


uint8_t Nw_eep_Send(uint8_t sla, uint16_t suba, uint8_t *s, uint16_t no)
{
    uint16_t i;

    
    osMutexWait( mid_iichal, INT32_MAX );
	
    Nw_Start_I2c(); //启动总线
    Nw_SendByte(sla); //发送器件地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    
    Nw_SendByte((suba>>8)); //发送器件子地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    Nw_SendByte(suba&0xFF); //发送器件子地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    
    for (i = 0; i < no; i++) {
        Nw_SendByte(*s); //发送数据
        if (Ack == IIC_NACK) {
            Nw_Stop_I2c();
			osMutexRelease( mid_iichal );
            return (IIC_NACK);
        }
        s++;
    }
    Nw_Stop_I2c(); //结束总线
	
	osMutexRelease( mid_iichal );
    return (IIC_ACK);
}

uint8_t Nw_eep_Rcv(uint8_t sla, uint16_t suba, uint8_t *s, uint16_t no)
{
    uint16_t i;

    osMutexWait( mid_iichal, INT32_MAX );
    
    Nw_Start_I2c(); //启动总线
    Nw_SendByte(sla); //发送器件地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
        osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    Nw_SendByte((suba>>8)); //发送器件子地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    Nw_SendByte(suba&0xFF); //发送器件子地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    
    Nw_Start_I2c();
    Nw_SendByte(sla + 1);
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    for (i = 0; i < no - 1; i++) {
        *s = Nw_RcvByte(); //发送数据
        Nw_Ack_I2c(IIC_ACK); //发送就答位
        s++;
    }
    *s = Nw_RcvByte();
    Nw_Ack_I2c(IIC_NACK); //发送非应位
    Nw_Stop_I2c(); //结束总线
	
	osMutexRelease( mid_iichal );
    return (IIC_ACK);
}

/**********************************************************************
向有子地址器件读取多字节数据函数               
函数原型: bit  ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
功能:从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件地址sla，
子地址suba，读出的内容放入s指向的存储区，读no个字节。如果返回1
表示操作成功，否则操作有误。
 **********************************************************************/
#define SHT85_CHIP_ID  0x88
const uint8_t SHT85_CMD[4] = {0x24, 0};

uint8_t Nw_SHT85_Read(uint8_t *s) {
    uint8_t i;
    uint8_t read_time_cnt;
    
    osMutexWait( mid_iichal, INT32_MAX );
    
    Nw_Start_I2c(); //启动总线
    Nw_SendByte(SHT85_CHIP_ID); //发送器件地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
        osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    
    Nw_SendByte(SHT85_CMD[0]); //发送器件子地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    
    Nw_SendByte(SHT85_CMD[1]); //发送器件子地址
    if (Ack == IIC_NACK) {
        Nw_Stop_I2c();
		osMutexRelease( mid_iichal );
        return (IIC_NACK);
    }
    Nw_Stop_I2c();
    
    osMutexRelease( mid_iichal );
    osDelay(3);

    for(read_time_cnt = 0; read_time_cnt <= 100; read_time_cnt++)
    {
        osMutexWait( mid_iichal, INT32_MAX );
        
        Nw_Start_I2c();
        Nw_SendByte(SHT85_CHIP_ID + 1);
        if (Ack == IIC_NACK) {
            Nw_Stop_I2c();
            osMutexRelease( mid_iichal );
            osDelay(3);
            
        }
        else
        {
            osMutexRelease( mid_iichal );
            break;
        }
    }
    
    if(read_time_cnt >= 100)
      return IIC_NACK;
    
    osMutexWait( mid_iichal, INT32_MAX );
    
    for (i = 0; i < 5; i++) {
        *s = Nw_RcvByte(); //发送数据
        Nw_Ack_I2c(IIC_ACK); //发送就答位
        s++;
    }
    *s = Nw_RcvByte();
    Nw_Ack_I2c(IIC_NACK); //发送非应位
    Nw_Stop_I2c(); //结束总线
	
	osMutexRelease( mid_iichal );
    return (IIC_ACK);
}


//////////////ji_add//////////////////
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
        delay_us(4);
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
        delay_us(2);
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
        delay_us(2);
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
        IIC_SDA=1;delay_us(1);	  /////////////ji_add
	SDA_IN();      //SDA设置为输入  
//	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>10)
		{
                        delay_us(1);	 
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

//初始化IIC接口
void AT24CXX_Init(void)
{
  IIC_Init();//IIC初始化
}
//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址	    
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	   
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA0+1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(50);	 
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
          AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	        temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}


//////////////ji_add_end///////////////
