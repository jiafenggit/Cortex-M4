#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_epi.h"
#include "inc/hw_ints.h"
#include "driverlib/epi.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/ssi.h"
#include "driverlib/fpu.h"
#include "utils/uartstdio.h"
#include "TFTinit/TFT_400x240_OTM4001A_16bit.h"
//#include "TFTinit/picture.h"
#include "TOUCHinit/TOUCH_TSC2046.h"
#include "EPIinit/EPIinit.h"
//#include "inc/hw_i2c.h"
//#include "driverlib/i2c.h"
#include "driverlib/comp.h"
#include "inc/hw_nvic.h"
#include "inc/tm4c1294ncpdt.h"

#define _NOP() _nop()

//*********************************************************************
//*********************************************************************
#define I2C0_MASTER_BASE 0x40020000
#define I2C0_SLAVE_BASE 0x40020000

//*********************************************************************
// 地址、寄存器等定义部分
//*********************************************************************
//*********************************************************************
//
// 设定slave（从）模块的地址。这是一个7-bit的地址加上RS位，具体形式如下:
//                      [A6:A5:A4:A3:A2:A1:A0:RS]
// RS位是一个指示位，如果RS=0，则说明是主发送数据，从接收数据；RS=1说明是主接收数据，从发送数据
//
//*********************************************************************
//U21控制4个米字管和特殊管脚的亮灭
#define I2C0_ADDR_TUBE_SEL	      0x30  //00110000
//U22控制米字管7~14管脚对应的码段
#define I2C0_ADDR_TUBE_SEG_LOW    0x32  //00110010
//U23控制米字管15~18管脚对应的码段
#define I2C0_ADDR_TUBE_SEG_HIGH  0x34   //00110100
//U24控制LED光柱

//PCA9557内部寄存器，也称子地址
#define PCA9557_REG_INPUT	 0x00
#define PCA9557_REG_OUTPUT	 0x01
#define PCA9557_REG_PolInver 0x02
#define PCA9557_REG_CONFIG	 0x03

//*************************************************************************************
 #define NUM 0
//IIC 接受数据临时缓冲区
unsigned char I2C_RECV_DATA[] =
				{
					0x00,
					0x00,
					0x00,
					0x00,
					0x00,
					0x00
				};


/*******************************************
		拉高 SDA 信号
********************************************/
void I2C_Set_sda_high( void )
{
	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,GPIO_PIN_3);  //拉高PB3
    _NOP();
    _NOP();
    return;
}

/*******************************************
		拉低SDA 信号
********************************************/
void I2C_Set_sda_low ( void )
{
	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,0X00000000);  //拉低PB3
    _NOP();
    _NOP();
    return;
}

/*******************************************
		拉高SCL 信号
********************************************/
void I2C_Set_scl_high( void )
{
	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,GPIO_PIN_2);  //拉高PB2
    _NOP();
    _NOP();
    return;
}

/*******************************************
		拉低SCL 信号
********************************************/
void I2C_Set_scl_low ( void )
{
	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,0X00000000);  //拉低PB2
    _NOP();
    _NOP();
    return;
}

/*******************************************
		IIC 信号结束信号函数
********************************************/
void I2C_STOP(void)
{
    int i;
    I2C_Set_sda_low();
    for(i = NUM;i > 0;i--);
    I2C_Set_scl_low();
    for(i = NUM;i > 0;i--);
    I2C_Set_scl_high();
    for(i = NUM;i > 0;i--);
    I2C_Set_sda_high();
    for(i = NUM+1;i > 0;i--);
    return;
}


/*******************************************
		IIC 信号初始化
********************************************/
void I2C_Initial( void )
{
    I2C_Set_scl_low();
    I2C_STOP();
    return;
}


/*******************************************
		IIC 信号起始信号函数
********************************************/
void I2C_START(void)
{
    int i;

    I2C_Set_sda_high();
    for(i = NUM;i > 0;i--);
    I2C_Set_scl_high();
    for(i = NUM;i > 0;i--);
    I2C_Set_sda_low();
    for(i = NUM;i > 0;i--);
    I2C_Set_scl_low();
    return;
}

/*******************************************
		IIC 获取应答函数
********************************************/
int  I2C_GetACK(void)
{
    int j;
    _NOP();
    _NOP();
    I2C_Set_scl_low();
    for(j = NUM;j> 0;j--);
    I2C_Set_scl_high();
    for(j = NUM;j> 0;j--);
    I2C_Set_sda_low();
    for(j = NUM;j > 0;j--);
    I2C_Set_scl_low();
    return 1;
}

/*******************************************
		IIC 设置应答函数
********************************************/
void I2C_SetNAk(void)
{
    I2C_Set_scl_low();
    I2C_Set_sda_high();
    I2C_Set_scl_high();
    I2C_Set_scl_low();
    return;
}

/*******************************************
		IIC 发送字节函数
		参数 	1：要发送字节值
		return ：无返回
********************************************/
void I2C_TxByte(unsigned char nValue)
{
    int i;
    int j;
    for(i = 0;i < 8;i++)
    {
    	if(nValue & 0x80)
    	    I2C_Set_sda_high();
    	else
    	    I2C_Set_sda_low();
    	for(j = NUM;j > 0;j--);
    	I2C_Set_scl_high();
    	nValue <<= 1;
    	for(j = NUM;j > 0;j--);
    	I2C_Set_scl_low();
    }

    return;
}

/*******************************************
		IIC 接收字节函数
		参数 		无
		return ：无返回
********************************************/
unsigned char  I2C_RxByte(void)
{
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_3);//配置PB3为输入口
    unsigned char nTemp=0 ;
    int i;

    I2C_Set_sda_high();

    _NOP();
    _NOP();
    _NOP();
    _NOP();
    for(i = 0;i < 8;i++)
    {
    	I2C_Set_scl_high(); //模拟SCL信号
    	if(GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_3) == 0x18) //依次判断PB3（SDA）输入
     	{
    	    nTemp |= (0x01 << (7-i));  //8位SDA数据哪一位为高就置1
    	}
    	I2C_Set_scl_low();
    	delay2();
    }

    return nTemp;

}

/*******************************************
		IIC 发送数组函数
	参数  	1 num : 发送字节数
	    2 device_addr : iic目标地址
	    3 *data	：发送数组地址
	return ：无返回
********************************************/
void i2c_write(int num, unsigned char device_addr,unsigned char *data)
{
    int i = 0;
    int count = num;
    unsigned char *send_data = data;
    unsigned char write_addr = device_addr;

    I2C_Set_scl_high();
    for(i = NUM;i > 0;i--);
    I2C_Set_sda_high();
    for(i = NUM;i > 0;i--);

    for(i = 0;i < count;i++)
    {
      I2C_START();           //模拟I2C写数据的时序
      I2C_TxByte(write_addr);
      I2C_GetACK();
      I2C_TxByte(send_data[i]);
      I2C_GetACK();
      i++;
      I2C_TxByte(send_data[i]);
      I2C_GetACK();
      I2C_STOP();
    }

}

/*******************************************
		IIC 读取数组函数
	参数  	1 num : 发送字节数
	    2 device_addr : iic目标地址
	    3 *data	：接收数组地址
	return ：无返回
********************************************/
void i2c_read(int num, unsigned char device_addr,unsigned char *data)
{
  int i = 0;
  int count = num;
  unsigned char *send_data = data;
  unsigned char read_addr = device_addr;

  I2C_Set_scl_high();
  for(i = NUM;i > 0;i--);
  I2C_Set_sda_high();
  for(i = NUM;i > 0;i--);

  for(i = 0; i < count;i++)
  {
    I2C_START();               //模拟I2C读数据
    I2C_TxByte((read_addr - 1));
    I2C_GetACK();
    I2C_TxByte(send_data[2*i]);
    I2C_GetACK();

    I2C_START();
    I2C_TxByte(read_addr);
    I2C_GetACK();

    I2C_RECV_DATA[i] = I2C_RxByte();
    data[2*i+1]=I2C_RECV_DATA[i];
    I2C_SetNAk();
    I2C_STOP();
  }

}

//*********************************************************************

//*********************************************************************
//******配置I2C0模块的IO引脚，**********************************************
void I2C0GPIOBEnable(void)
{	// Enable GPIO portB containing the I2C pins (PB2&PB3)
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2|GPIO_PIN_3);

}

//******配置PCA9557芯片中连接米字管的各引脚为输出***********************************
void I2C0DeviceInit(void)
{
	unsigned char dataBuf[2] = {PCA9557_REG_CONFIG, 0x00};
	i2c_write(2,I2C0_ADDR_TUBE_SEL,dataBuf);
	i2c_write(2,I2C0_ADDR_TUBE_SEG_LOW,dataBuf);
	i2c_write(2,I2C0_ADDR_TUBE_SEG_HIGH,dataBuf);

}

//*******设置米字管的管选信号**************************************************
void I2C0TubeSelSet(char data)
{   //选择1、2、3、4、5哪个米字管亮
	unsigned char dataBuf[2] = {PCA9557_REG_OUTPUT, data};
	i2c_write(2,I2C0_ADDR_TUBE_SEL,dataBuf);
}
//*******点亮米字管的相应码段**************************************************
void I2C0TubeLowSet(char data)
{  //点亮7-14管脚对应的码段
	unsigned char dataBuf[2] = {PCA9557_REG_OUTPUT, data};
	i2c_write(2,I2C0_ADDR_TUBE_SEG_LOW,dataBuf);
}
void I2C0TubeHighSet(char data)
{  //点亮15-18管脚对应的码段
	unsigned char dataBuf[2] = {PCA9557_REG_OUTPUT, data};
	i2c_write(2,I2C0_ADDR_TUBE_SEG_HIGH,dataBuf);
}

//********预设码段值，方便查找************************************************
static const char tubeCodeTable[10][2]=
		{ //  SegmLow, SegHigh
			{	0x10,	0x3E	},		// 	    	0
			{	0x00,	0x18	},		// 	        1
			{	0x70,	0x2C	},		//      	2
            {	0x70,	0x26	},		//       	3
            {	0x60,	0x32	},		//      	4
            {	0x70,	0x16	},		//     		5
            {	0x70,	0x1E	},		//     		6
            {	0x00,	0x26	},		//     		7
            {	0x70,	0x3E	},		//     		8
            {	0x70,	0x36	},	    //     		9
//            {	0x60,	0x3E	},	    //     		a
		};
unsigned char a[2];
void setnumber(char value)
{
    char b;
    b=value;
    switch(b){
    case 0:{ a[0]=tubeCodeTable[0][0];a[1]=tubeCodeTable[0][1];break;}
    case 1:{ a[0]=tubeCodeTable[1][0];a[1]=tubeCodeTable[1][1];break;}
    case 2:{ a[0]=tubeCodeTable[2][0];a[1]=tubeCodeTable[2][1];break;}
    case 3:{ a[0]=tubeCodeTable[3][0];a[1]=tubeCodeTable[3][1];break;}
    case 4:{ a[0]=tubeCodeTable[4][0];a[1]=tubeCodeTable[4][1];break;}
    case 5:{ a[0]=tubeCodeTable[5][0];a[1]=tubeCodeTable[5][1];break;}
    case 6:{ a[0]=tubeCodeTable[6][0];a[1]=tubeCodeTable[6][1];break;}
    case 7:{ a[0]=tubeCodeTable[7][0];a[1]=tubeCodeTable[7][1];break;}
    case 8:{ a[0]=tubeCodeTable[8][0];a[1]=tubeCodeTable[8][1];break;}
    case 9:{ a[0]=tubeCodeTable[9][0];a[1]=tubeCodeTable[9][1];break;}
    	     }
}
void I2C0Tubeset(void)
{
	I2C0TubeLowSet(a[0]);
	I2C0TubeHighSet(a[1]);

}
void showvalue(char value)
{
    setnumber(value);
    I2C0Tubeset();
}

void GPIOInitial(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOD);//
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);

    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOP);//
    GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, GPIO_PIN_2);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOL);//
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

}
void GPIOIntInitial(void)
{

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOD);//
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE,GPIO_PIN_0);
    GPIOIntTypeSet(GPIO_PORTD_BASE,GPIO_PIN_0,GPIO_LOW_LEVEL);
    GPIOIntEnable(GPIO_PORTD_BASE,GPIO_INT_PIN_0);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPION);//
    GPIOPinTypeGPIOInput(GPIO_PORTN_BASE,GPIO_PIN_2);
    GPIOIntTypeSet(GPIO_PORTN_BASE,GPIO_PIN_2,GPIO_LOW_LEVEL);
    GPIOIntEnable(GPIO_PORTN_BASE,GPIO_INT_PIN_2);
}

int stop=0;
int end=0;
int start=0;
void
GPION(void)
{
	unsigned long Status;
	Status=GPIOIntStatus(GPIO_PORTN_BASE,true);
    if(Status==GPIO_INT_PIN_2)
    {
    	stop=1;
    }
    GPIOIntClear(GPIO_PORTD_BASE,Status);
}

void
GPIOD(void)
{
	unsigned long Status;
	Status=GPIOIntStatus(GPIO_PORTD_BASE,true);
    if(Status==GPIO_INT_PIN_0)
    {
//    	GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_2 , 0xff);
//        SysCtlDelay(500*(16000000/3000));//2
//        GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_2 , 0x00);
//        SysCtlDelay(500*(16000000/3000));//2
    	start=1;
    	stop=0;
    	end=0;
    }
    GPIOIntClear(GPIO_PORTD_BASE,Status);


}



// System clock rate in Hz.
//
//*****************************************************************************
uint32_t g_ui32SysClock;

extern uint32_t GetData[6];

uint32_t TouchXData[6];
uint32_t TouchYData[6];
uint32_t TouchZData[6];   //Z is for pressure measurement
//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}
/*********************************************
 *********************************************
 *********************************************
 ******************绘制方块儿******************
 *********************************************
 *********************************************
 *********************************************/
void drawrect(int x,int y,int h ,int n)
{
	int a;
	if(x==1)
		a=30;
	if(x==2)
		a=90;
	if(x==3)
		a=150;
	if(x==0)
		a=210;
	if(n==1)
	{
		TFTLCD_FillBlock(a-28,a+28,y-h,y+h,WHITE);
	}
	if(n==2)
	{
		TFTLCD_FillBlock(a-28,a+28,y-h,y+h,GREEN);
	}
	if(n==3)
	{
		TFTLCD_FillBlock(a-28,a+28,y-h,y+h,WHITE);
	}
	if(n==4)
	{
		TFTLCD_FillBlock(a-28,a+28,y-h,y+h,RED);
	}
}
//================================================================================================
//	实现功能：	画一条水平线
//	输入参数：	x0:横坐标起点,x1:横坐标终点,y:纵坐标,color:指定颜色
//================================================================================================
void TFTLCD_DrawHorizontalLine(uint32_t x0,uint32_t x1,uint32_t y,uint32_t color)
{
	uint32_t i=0,PointNum = 0;
	PointNum = x1-x0;
	for(i=0;i<PointNum;i++)
	{
		TFTLCD_DrawPoint(x0+i,y,color);
	}
}
//================================================================================================
//	实现功能：	画一条垂直线
//	输入参数：	y0:纵坐标起点,y1:纵坐标终点,x:横坐标,color:指定颜色
//================================================================================================
void TFTLCD_DrawVerticalLine(uint32_t y0,uint32_t y1,uint32_t x,uint32_t color)
{
	uint32_t i=0,PointNum = 0;
	PointNum = y1-y0;
	for(i=0;i<PointNum;i++)
	{
		TFTLCD_DrawPoint(x,y0+i,color);
	}
}
void clear(int x,int y,int h)
{

	int a;
		if(x==1)
			a=30;
		if(x==2)
			a=90;
		if(x==3)
			a=150;
		if(x==0)
			a=209;

		TFTLCD_FillBlock(a-30,a+30,y-h,y+h,BLACK);

}
void delay2()
{
    int ui32Loop0;
    for(ui32Loop0=0;ui32Loop0<50  ;ui32Loop0++)  //delay
    {;}
}
void miziguan_show(jishu)
{
	int m1=0;
	int m2=0;
	m1=jishu%10;
	m2=jishu/10;

//	  {	0x10,	0x3E	},		// 	    	0
//	  {	0x00,	0x18	},		// 	        1
//	  {	0x70,	0x2C	},		//      	2
//    {	0x70,	0x26	},		//       	3
//    {	0x60,	0x32	},		//      	4
//    {	0x70,	0x16	},		//     		5
//    {	0x70,	0x1E	},		//     		6
//    {	0x00,	0x26	},		//     		7
//    {	0x70,	0x3E	},		//     		8
//    {	0x70,	0x36	},	    //     		9
//    {	0x60,	0x3E	},	    //     		a
	if(m2>0)
	{
		I2C0TubeSelSet(0x1E); //选择第一个管
			switch(m2)
			{
			case 0:
			{
				I2C0TubeLowSet(0x10); //8-14段
				I2C0TubeHighSet(0x3E); //15-18段，点亮0
				break;
			}
			case 1:
			{
				I2C0TubeLowSet(0x00); //8-14段
				I2C0TubeHighSet(0x18); //15-18段，点亮1
				break;
			}
			case 2:
			{
				I2C0TubeLowSet(0x70); //8-14段
				I2C0TubeHighSet(0x2C); //15-18段，点亮2
				break;
			}
			case 3:
			{
				I2C0TubeLowSet(0x70); //8-14段
				I2C0TubeHighSet(0x26); //15-18段，点亮3
				break;
			}
			case 4:
			{
				I2C0TubeLowSet(0x60); //8-14段
				I2C0TubeHighSet(0x32); //15-18段，点亮4
				break;
			}
			case 5:
			{
				I2C0TubeLowSet(0x70); //8-14段
				I2C0TubeHighSet(0x16); //15-18段，点亮5
				break;
			}
			case 6:
			{
				I2C0TubeLowSet(0x70); //8-14段
				I2C0TubeHighSet(0x1E); //15-18段，点亮6
				break;
			}
			case 7:
			{
				I2C0TubeLowSet(0x00); //8-14段
				I2C0TubeHighSet(0x26); //15-18段，点亮7
				break;
			}
			case 8:
			{
				I2C0TubeLowSet(0x70); //8-14段
				I2C0TubeHighSet(0x3E); //15-18段，点亮8
				break;
			}
			case 9:
			{
				I2C0TubeLowSet(0x70); //8-14段
				I2C0TubeHighSet(0x36); //15-18段，点亮9
				break;
			}
			}
			SysCtlDelay(5000000);  //延时
	}

	I2C0TubeSelSet(0x3C); //选择第二个管
	switch(m1)
	{
	case 0:
	{
		I2C0TubeLowSet(0x10); //8-14段
		I2C0TubeHighSet(0x3E); //15-18段，点亮0
		break;
	}
	case 1:
	{
		I2C0TubeLowSet(0x00); //8-14段
		I2C0TubeHighSet(0x18); //15-18段，点亮1
		break;
	}
	case 2:
	{
		I2C0TubeLowSet(0x70); //8-14段
		I2C0TubeHighSet(0x2C); //15-18段，点亮2
		break;
	}
	case 3:
	{
		I2C0TubeLowSet(0x70); //8-14段
		I2C0TubeHighSet(0x26); //15-18段，点亮3
		break;
	}
	case 4:
	{
		I2C0TubeLowSet(0x60); //8-14段
		I2C0TubeHighSet(0x32); //15-18段，点亮4
		break;
	}
	case 5:
	{
		I2C0TubeLowSet(0x70); //8-14段
		I2C0TubeHighSet(0x16); //15-18段，点亮5
		break;
	}
	case 6:
	{
		I2C0TubeLowSet(0x70); //8-14段
		I2C0TubeHighSet(0x1E); //15-18段，点亮6
		break;
	}
	case 7:
	{
		I2C0TubeLowSet(0x00); //8-14段
		I2C0TubeHighSet(0x26); //15-18段，点亮7
		break;
	}
	case 8:
	{
		I2C0TubeLowSet(0x70); //8-14段
		I2C0TubeHighSet(0x3E); //15-18段，点亮8
		break;
	}
	case 9:
	{
		I2C0TubeLowSet(0x70); //8-14段
		I2C0TubeHighSet(0x36); //15-18段，点亮9
		break;
	}
	}
	SysCtlDelay(2000000);  //延时
}

void main()
{
	uint16_t ui32Loop = 0,temp=0;
    //
    // The FPU should be enabled because some compilers will use floating-
    // point registers, even for non-floating-point code.  If the FPU is not
    // enabled this will cause a fault.  This also ensures that floating-
    // point operations could be added to this application and would work
    // correctly and use the hardware floating-point unit.  Finally, lazy
    // stacking is enabled for interrupt handlers.  This allows floating-
    // point instructions to be used within interrupt handlers, but at the
    // expense of extra stack usage.
    //
    FPUEnable();
    FPULazyStackingEnable();
    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);
    GPIOInitial();
    GPIOIntInitial();
    IntPrioritySet(INT_GPION, 0x20);
    IntPrioritySet(INT_GPIOD, 0x80);
    IntEnable(INT_GPION);
    IntEnable(INT_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);//使能GPIOF
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);//使能GPIOL
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);//配置PF1、PF2、PF3为输出
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);//配置PL1、PL2、PL3、PL4为输出
//    SysTickPeriodSet(g_ui32SysClock / 40);
//    SysTickIntEnable();
//    SysTickEnable();

    ConfigureUART();
    EPIGPIOinit();
    TOUCH_TSC2046init(g_ui32SysClock);

    I2C0GPIOBEnable();//配置I2C0模块的IO引脚
    I2C0DeviceInit();

    GPIOIntEnable(GPIO_PORTB_BASE,GPIO_INT_PIN_0);
    GPIOIntTypeSet(GPIO_PORTB_BASE,GPIO_PIN_0,GPIO_FALLING_EDGE);

    UARTprintf("TFTLCD touch screen test\n");
    UARTprintf("EPI Type: host-bus 16-bit interface\n");
    UARTprintf("SSI Type: SSI0 8bit\n");
    GPIO_PORTM_DIR_R = 0x28;
    GPIO_PORTM_DEN_R = 0x28;
    int c[6];
    int j,ii;
    int n=0;
//    int end=0;
    int x[6];
    int y[6];
    int h[6];
    int avg=0;
    int i;
    int jishu=0;
    int miziguan[2];
    int m[6];
    int count=0;
    int flag=1;
    int z=10000;
//    int start=0;
    for(i=0;i<6;i++)
    {
    	c[i]=1;
    	m[i]=1;
    	y[i]=0;
    }

    TFT_400x240_OTM4001Ainit(g_ui32SysClock);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
	//SSIDataPut(SSI0_BASE,0xd0);

	//TFTLCD_ShowString(50,10,"GC211 TFTLCD test!",YELLOW,LIGHTBLUE);
	SSIDataPut(SSI0_BASE,0xd0);
    while(1)
    {
    	while(stop==1)
    	{
    		if(flag==1)
    		{
        		TFTLCD_FillBlock(0,240,0,400,BLACK);
    		}
    		TFTLCD_DrawHorizontalLine(0,240,0,RED);
    		TFTLCD_DrawHorizontalLine(0,240,40,RED);
    		TFTLCD_ShowString(40,180,"You have stopped it!",YELLOW,LIGHTGREEN);
    		TFTLCD_ShowString(10,20,"Push here to continue game!",YELLOW,LIGHTGREEN);
    		flag=0;
//			for(ui32Loop=0;ui32Loop<=5;ui32Loop++)
//			{
//				SSIDataPut(SSI0_BASE,0x90);
//				SysCtlDelay(3);
//				SSIDataGet(SSI0_BASE,&TouchXData[ui32Loop]);
//				SysCtlDelay(3);
//				SSIDataPut(SSI0_BASE,0xd0);
//				SysCtlDelay(3);
//				SSIDataGet(SSI0_BASE,&TouchYData[ui32Loop]);
//				SysCtlDelay(3);
//			}
//			TouchXData[5] = (TouchXData[0]+TouchXData[1]+TouchXData[2]+TouchXData[3]+TouchXData[4])/5;
//			TouchYData[5] = (TouchYData[0]+TouchYData[1]+TouchYData[2]+TouchYData[3]+TouchYData[4])/5;
//
//           for(n=0;n<5;n++)
//           {
//           	avg+=(TouchXData[5]-TouchXData[n])*(TouchXData[5]-TouchXData[n]);
//           	avg+=(TouchYData[5]-TouchYData[n])*(TouchYData[5]-TouchYData[n]);
//           }
//           avg=avg/5;
//           TOUCH_PointAdjust(&TouchXData[5], &TouchYData[5]);
//           if((TouchXData[5]<=240)&&(TouchYData[5]<=40))
//           {
//        	   start=1;
//        	   stop=0;
//        	   end=0;
//           }
    	}

    	while(start==0)
    	{
    		TFTLCD_DrawHorizontalLine(60,180,130,RED);
    		TFTLCD_DrawHorizontalLine(60,180,210,RED);
    		TFTLCD_DrawVerticalLine(130,210,60,RED);
    		TFTLCD_DrawVerticalLine(130,210,180,RED);
    		TFTLCD_ShowString(95,165,"START!",YELLOW,LIGHTGREEN);//绘制开始页面窗口
    		for(ui32Loop=0;ui32Loop<=5;ui32Loop++)
    		{
    			SSIDataPut(SSI0_BASE,0x90);
    			SysCtlDelay(3);
    			SSIDataGet(SSI0_BASE,&TouchXData[ui32Loop]);
    			SysCtlDelay(3);
    			SSIDataPut(SSI0_BASE,0xd0);
    			SysCtlDelay(3);
    			SSIDataGet(SSI0_BASE,&TouchYData[ui32Loop]);
    			SysCtlDelay(3);
    		}
    		TouchXData[5] = (TouchXData[0]+TouchXData[1]+TouchXData[2]+TouchXData[3]+TouchXData[4])/5;
    		TouchYData[5] = (TouchYData[0]+TouchYData[1]+TouchYData[2]+TouchYData[3]+TouchYData[4])/5;
    		for(n=0;n<5;n++)
    		{
    			avg+=(TouchXData[5]-TouchXData[n])*(TouchXData[5]-TouchXData[n]);
    			avg+=(TouchYData[5]-TouchYData[n])*(TouchYData[5]-TouchYData[n]);
    		}
    		avg=avg/5;
    		TOUCH_PointAdjust(&TouchXData[5], &TouchYData[5]);
    		if((TouchXData[5]<=240)&&(TouchYData[5]<=400) & avg<500)
    		{
    		    TFTLCD_DrawPoint(TouchXData[5],TouchYData[5],BLUE);
    		    if((TouchXData[5]<180 & TouchXData[5]>60) && (TouchYData[5]<210 & TouchYData[5]>130))
    		    {
    		    	start=1;
    		    	TFTLCD_FillBlock(0,240,0,400,BLACK);
    		    }
    		}
            for(i=0;i<6;i++)
            {
            	c[i]=1;
            	m[i]=1;
            	y[i]=0;
            }
    	}
    	while(end==0 && start==1 && stop==0)
    	{
    		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0);//三个LED灯关闭
    		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0);
    		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0);
    		if(flag==0)
    		{
    			TFTLCD_FillBlock(0,240,0,400,BLACK);
    			flag=1;
    		}
    		TFTLCD_ShowString(30,20,"Push here to stop game!",YELLOW,LIGHTGREEN);
    		TFTLCD_DrawHorizontalLine(0,240,0,RED);
    		TFTLCD_DrawHorizontalLine(0,240,40,RED);
    		TFTLCD_DrawHorizontalLine(0,240,100,RED);
    		TFTLCD_DrawHorizontalLine(0,240,160,RED);
    		TFTLCD_DrawHorizontalLine(0,240,220,RED);
    		TFTLCD_DrawHorizontalLine(0,240,280,RED);
    		TFTLCD_DrawHorizontalLine(0,240,340,RED);
    		TFTLCD_DrawHorizontalLine(0,240,399,RED);
    		TFTLCD_DrawVerticalLine(0,400,0,RED);
    		TFTLCD_DrawVerticalLine(40,400,60,RED);
    		TFTLCD_DrawVerticalLine(40,400,120,RED);
    		TFTLCD_DrawVerticalLine(40,400,180,RED);
    		TFTLCD_DrawVerticalLine(0,400,239,RED);
    		for(i=0; i<6; i++)
    		{
        		if(i==count%6)
        		{
        			x[i]=rand()%4;
        			y[i]=70;
        			h[i]=28;
        			m[i]=1;
        			c[i]=1;
        		}
        		if(y[i]==0)
        			h[i]=0;
    		}
    		 for(i=0;i<6;i++)
    			 if(y[i]!=0 && i!=count%6)
    				 y[i]=y[i]+60;
    		 for(i=0;i<6;i++)
    			 drawrect(x[i],y[i],h[i],m[i]);
    		 count++;


       	for(j=0;j<z;j++)
       	{
			for(ui32Loop=0;ui32Loop<=5;ui32Loop++)
			{
				SSIDataPut(SSI0_BASE,0x90);
				SysCtlDelay(3);
				SSIDataGet(SSI0_BASE,&TouchXData[ui32Loop]);
				SysCtlDelay(3);
				SSIDataPut(SSI0_BASE,0xd0);
				SysCtlDelay(3);
				SSIDataGet(SSI0_BASE,&TouchYData[ui32Loop]);
				SysCtlDelay(3);
			}
			TouchXData[5] = (TouchXData[0]+TouchXData[1]+TouchXData[2]+TouchXData[3]+TouchXData[4])/5;
			TouchYData[5] = (TouchYData[0]+TouchYData[1]+TouchYData[2]+TouchYData[3]+TouchYData[4])/5;

           for(n=0;n<5;n++)
           {
           	avg+=(TouchXData[5]-TouchXData[n])*(TouchXData[5]-TouchXData[n]);
           	avg+=(TouchYData[5]-TouchYData[n])*(TouchYData[5]-TouchYData[n]);
           }
           avg=avg/5;
           TOUCH_PointAdjust(&TouchXData[5], &TouchYData[5]);

			if((TouchXData[5]<=240)&&(TouchYData[5]<=400) & avg<500)
			{
				if((TouchXData[5]<=240)&&(TouchYData[5]<=40))
				{
					stop=1;
					break;
				}
				TFTLCD_DrawPoint(TouchXData[5],TouchYData[5],BLACK);
				int waijie=0;
				for(i=0;i<6;i++)
				{
					if(TouchYData[5]>y[i]-33   && TouchYData[5]<y[i]+33)
						switch(x[i])
						{
						case 0:
						{
							if(TouchXData[5]>210-33   && TouchXData[5]<210+33)
							{
								c[i]=2;
								waijie++;
								break;
							}
						}
						case 1 :
						{
							if(TouchXData[5]>30-28   && TouchXData[5]<30+33)
							{
								c[i]=2;
								waijie++;
								break;
							}
						}
						case 2 :
						{
							if(TouchXData[5]>90-33   && TouchXData[5]<90+33)
							{
								c[i]=2;
								waijie++;
								break;
							}
						}
						case 3 :
						{
							if(TouchXData[5]>150-33   && TouchXData[5]<150+33)
							{
								c[i]=2;
								waijie++;
								break;
							}
						}
					}
				}
				if(waijie==0)
				{
					end=1;
				    //GPIO_PORTM_DATA_R|=0x20;//蜂鸣器
				}
			}
       	}
       	GPIO_PORTM_DATA_R&=0x00;
       	for(i=0;i<6;i++)
       	{
       		if(c[i]==2 && m[i]!=2)
       			jishu++;
       	}
       	for(i=0;i<6;i++)
       	{
       		m[i]=c[i];
       	}
       	if(jishu<10)
       	    	{
       	        	z=z-jishu*100;
       	    	}

       	miziguan_show(jishu);

    	        for (i=0;i<6;i++)
    	        {
    	        	clear(x[i],y[i],h[i]);
    	        }

    	        for(i=0;i<6;i++)
    	        {
    	        	if(y[i]+30==400)
    	        		if(m[i]==1)
    	        		{
    	        			end=1;
    	        			for(ii=0;ii<4;ii++)
    	        			{
    	        				if(ii==0)
    	        					y[ii]=1;
    	        				else
    	        					y[ii]=0;
    	        			}
    	        		}
    	        		else
    	        			y[i]=1;
    	        }
    	}

    	if(end==1)
        {
    		TFTLCD_FillBlock(0,240,0,400,BLACK);
    		TFTLCD_ShowString(65,160,"Designed by YHL",RED,LIGHTBLUE);
    		TFTLCD_ShowString(80,180,"GAME OVER!",RED,LIGHTBLUE);
    		TFTLCD_DrawHorizontalLine(60,180,240,RED);
    		TFTLCD_DrawHorizontalLine(60,180,320,RED);
    		TFTLCD_DrawVerticalLine(240,320,60,RED);
    		TFTLCD_DrawVerticalLine(240,320,180,RED);
    		TFTLCD_ShowString(90,270,"RETURN!",RED,LIGHTBLUE);
    		jishu=0;
    		z=10000;
    		miziguan[0]=jishu%10;
    		miziguan[1]=jishu/10;
    		I2C0TubeSelSet(0xDF);
    		showvalue(miziguan[1]);
    		I2C0TubeSelSet(0xFD);
    		showvalue(miziguan[0]);
    		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1); //PF1输出高，点亮LED0
    		SysCtlDelay(10000*(10000000/3000));         //延时n*100ms
    		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0); //PF1输出低，关闭LED0

    		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,GPIO_PIN_2); //PF2输出高，点亮LED1
    		SysCtlDelay(10000*(10000000/3000));         //延时n*100ms
    		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0); //PF2输出低，点亮LED1

    		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,GPIO_PIN_3);
    		SysCtlDelay(10000*(10000000/3000));         //延时n*100ms
    		GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0);

    		GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,GPIO_PIN_0);
    		SysCtlDelay(10000*(10000000/3000));         //延时n*100ms
    		GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0);

    		GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,GPIO_PIN_1);
    		SysCtlDelay(10000*(10000000/3000));         //延时n*100ms
    		GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0);

    		GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_2,GPIO_PIN_2);
    		SysCtlDelay(10000*(10000000/3000));         //延时n*100ms
    		GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_2,0);

    		GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,GPIO_PIN_3);
    		SysCtlDelay(10000*(10000000/3000));         //延时n*100ms
    		GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,0);

    		GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_4,GPIO_PIN_4);
    		SysCtlDelay(10000*(10000000/3000));         //延时n*100ms
    		GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_4,0);
        }


    	while(end==1)
    	{
    		for(ui32Loop=0;ui32Loop<=5;ui32Loop++)
    		{
    			SSIDataPut(SSI0_BASE,0x90);
    		    SysCtlDelay(3);
    		    SSIDataGet(SSI0_BASE,&TouchXData[ui32Loop]);
    		    SysCtlDelay(3);
    		    SSIDataPut(SSI0_BASE,0xd0);
    		    SysCtlDelay(3);
    		    SSIDataGet(SSI0_BASE,&TouchYData[ui32Loop]);
    		    SysCtlDelay(3);
    		}
    		TouchXData[5] = (TouchXData[0]+TouchXData[1]+TouchXData[2]+TouchXData[3]+TouchXData[4])/5;
    		TouchYData[5] = (TouchYData[0]+TouchYData[1]+TouchYData[2]+TouchYData[3]+TouchYData[4])/5;

    		for(n=0;n<5;n++)
    		{
    		    avg+=(TouchXData[5]-TouchXData[n])*(TouchXData[5]-TouchXData[n]);
    		    avg+=(TouchYData[5]-TouchYData[n])*(TouchYData[5]-TouchYData[n]);
    		}
    		avg=avg/5;
    		TOUCH_PointAdjust(&TouchXData[5], &TouchYData[5]);
    		if((TouchXData[5]<=240)&&(TouchYData[5]<=400) & avg<500)
    		    		{
    		                TFTLCD_DrawPoint(TouchXData[5],TouchYData[5],BLUE);
    		                if((TouchXData[5]<180 & TouchXData[5]>60) && (TouchYData[5]<320 & TouchYData[5]>240))
    		    			{
    		                	end=0;
    		    				start=0;
    		    				TFTLCD_FillBlock(0,240,0,400,BLACK);
    		    			}

    		    		}
    	}
    	}
  }
