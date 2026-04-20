
/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/


#ifndef		__PINCFG_H
#define		__PINCFG_H

#include "sys.h"

//------------------------------pin function define---------------------------//
#define LED_CONNE     PDout(7)
#define LED_ERROR     PDout(6)
#define LED_CPU_RUN   PBout(3)
#define LED_ON        0
#define LED_OFF       1


#define IO_I_SD_ST    PEin(15)

#define EEP_WP        PBout(4)
#define EEP_WP2       PBout(5)
#define EEP_SDA       PB7
#define EEP_SCL       PB6

#define BUZZER        PDout(9)
#define BuzzerOn()    BUZZER = 1
#define BuzzerOff()   BUZZER = 0

#define USB           PA11_PA12

//板级输出
#define _OUT_JR1             PAout(10)      
#define _OUT_JR2             PAout(9)  
#define _OUT_JR3             PAout(8)  

#define _OUT_VALAE1             PDout(10)      
#define _OUT_VALAE2             PDout(11)  
#define _OUT_VALAE3             PDout(12)  
#define _OUT_VALAE4             PDout(13) 
#define _OUT_VALAE5             PDout(14) 

#define _OUT_JK_AC220           PEout(14)   

//输出功能映射
#define OUT_CO2_CRL1           _OUT_VALAE1         
#define OUT_CO2_CRL2           _OUT_VALAE2       
#define OUT_COOL_PUMP          _OUT_VALAE3 
#define OUT_WATER_PUMP         _OUT_VALAE4    
#define OUT_FSCRL3             _OUT_VALAE5

#define OUT_FRAME_HEAT    _OUT_JR1
#define OUT_DOOR_HEAT     _OUT_JR2
#define OUT_YL_HEAT       _OUT_JR3

#define SSR_OUT_ENABLE       1
#define SSR_OUT_DISABLE      0
#define JK_OUT_ENABLE        1
#define JK_OUT_DISABLE       0

//板级输入    
#define _INPUT1            PBin(12)
#define _INPUT2            PBin(13)
#define _INPUT3            PBin(14)
#define _INPUT4            PBin(15)
#define _INPUT5            PDin(8)

#define IN_CO2TANK1        _INPUT1
#define IN_CO2TANK2        _INPUT2
#define IN_DOOR_SW         _INPUT3
#define IN_RHW_SW          _INPUT4
#define IN_YL              _INPUT5

#define IN_EN_ST           0

#define JK_OUT_AC220_en()       _OUT_JK_AC220=1
#define JK_OUT_AC220_dis()      _OUT_JK_AC220=0

//串口
//modbus 接口
#define RS232P1       UART6       //RS232 modbus-slave         modbus  mbportx
#define RS232P2       UART2       //RS232 modbus-master-dwin   modbus  mbporty

//232串口
#define RS232P3       UART3       //RS232 print
#define RS232P8       UART8       //RS232 gprs

//传感器接口  //注：此端口采用方向与TX信号同步，不可用于高速通信
#define R485P1      UART7       //RS458 sen                    modbus  mbportz
#define R485P2      R485P1


//以下管脚悬空配置为输出低
//PB0-PB5
//PB8-PB11
//PB14
//PD6
//PD10-PD12
//PE3-PE5
//PE10
//PE12
//PC2-PC4
//PC7
//PA0
//PA15

//----------------------------------------------------------------------------
#define NOP1()  __NOP()
#define NOP2()  NOP1();NOP1()
#define NOP3()  NOP2();NOP1()
#define NOP4()  NOP3();NOP1()
#define NOP5()  NOP4();NOP1()
#define NOP6()  NOP5();NOP1()
#define NOP7()  NOP6();NOP1()
#define NOP8()  NOP7();NOP1()
#define NOP9()  NOP8();NOP1()
#define NOP10() NOP9();NOP1()
#define NOP11() NOP10();NOP1()
#define NOP12() NOP11();NOP1()
#define NOP13() NOP12();NOP1()
#define NOP14() NOP13();NOP1()
#define NOP15() NOP14();NOP1()
#define NOP16() NOP15();NOP1()
#define NOP17() NOP16();NOP1()
#define NOP18() NOP17();NOP1()
#define NOP19() NOP18();NOP1()
#define NOP20() NOP19();NOP1()
#define NOP21() NOP20();NOP1()
#define NOP22() NOP21();NOP1()
#define NOP23() NOP22();NOP1()
#define NOP24() NOP23();NOP1()
#define NOP25() NOP24();NOP1()
#define NOP26() NOP25();NOP1()
#define NOP27() NOP26();NOP1()
#define NOP28() NOP27();NOP1()
#define NOP29() NOP28();NOP1()
#define NOP30() NOP29();NOP1()
#define NOP31() NOP30();NOP1()
#define NOP32() NOP31();NOP1()
#define NOP33() NOP32();NOP1()
#define NOP34() NOP33();NOP1()
#define NOP35() NOP34();NOP1()
#define NOP36() NOP35();NOP1()
#define NOP37() NOP36();NOP1()
#define NOP38() NOP37();NOP1()
#define NOP39() NOP38();NOP1()
#define NOP40() NOP39();NOP1()
#define NOP(N)  NOP##N()


#endif
