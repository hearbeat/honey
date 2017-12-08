 #include "DHT11.h" 
 #include <stdio.h>
 #include "string.h"
 extern unsigned char  F16T,F16RH;		//温湿度数据
sbit LED1=P3^7;
sbit LED2=P3^6;
#define uchar unsigned char 
#define uint unsigned int
unsigned char idata Rxbuff[50],Rxnum;
unsigned char	Recwifi_data[5];
char *strx=0;
char *strx1=0;
char clinetid;//连接ID
unsigned char getflag;//获取标志
void Get_Clinet(void);
void InitUART(void)			 //这是串口的基本配置，配置他的波特率是9600.这些参数都是标准的。
{
    TMOD = 0x20;
    SCON = 0x50;
    TH1 = 0xFD;
    TL1 = TH1;
    PCON = 0x00;
    EA = 1;
    ES = 1;
    TR1 = 1;
}
 void Clear_Buffer(void)//清空缓存
{
		int i;
	
	//	for(i=0;i<50;i++)
	//	Rxbuff[i]=0;//
		Rxnum=0;

}
 
/******************************************************************************/
void delayms(unsigned int x)
{
	unsigned int i;
	while(x--)
		for(i=125;i>0;i--);
}
/*
 * UART 发送一字节
*/
void UART_send_byte(char dat)
{
	SBUF = dat;
	while (TI == 0);
	TI = 0;
}

/*
 * UART 发送字符串
*/
void Send_Str(unsigned char *buf)
{
	while (*buf != '\0')
	{
		UART_send_byte(*buf++);
	}
}

void ESP8266_SERVER(void)//服务器建立
{

	Send_Str("AT\r\n");
	delayms(500);
	Send_Str("AT\r\n");
	delayms(500);
	Send_Str("AT\r\n");
	delayms(500);
	Send_Str("ATE0\r\n");	  //关闭回显
	delayms(500);
   	Send_Str("AT+CWSAP=\042ESP8266_001\042,\0421234567890\042,5,3\r\n");	  //设置显示名称和密码
	delayms(500);
	Send_Str("AT+CIPMUX=1\r\n");//启动多连接，建立服务器都需要配置
	delayms(500);	
	Clear_Buffer();	
	Send_Str("AT+CIPSERVER=1,8888\r\n");//建立服务器
	delayms(500);	
	strx=strstr((const char*)Rxbuff,(const char*)"OK");//等待建立完成
	while(strx==NULL)
	{		Clear_Buffer();	
			delayms(500);
			strx=strstr((const char*)Rxbuff,(const char*)"OK");//等待建立完成
	}
	Clear_Buffer();	

}
void Get_Clinet(void)//判断是否获取到了客户端接入
{

	   uchar i;
     
		strx=strstr((const char*)Rxbuff,(const char*)"CONNECT");//返回连接值
			if(strx)
		{
		  Clear_Buffer();	
			getflag=1;
		}
		strx=strstr((const char*)Rxbuff,(const char*)"CLOSE");//返回断开连接
			if(strx)
		{
		  Clear_Buffer();	
			getflag=0;
		}
		if(getflag)
		{
				strx=strstr((const char*)Rxbuff,(const char*)"+IPD");//有数据返回
			  if(strx)//正常获取数据
				 {
					 clinetid=strx[5];
					 strx=strstr((const char*)Rxbuff,(const char*)":");//有数据返回
					 for(i=0;i<2;i++)
					 Recwifi_data[i]=strx[1+i];		  //获取手机对LED的控制
					  if( Recwifi_data[0]=='1'&&   Recwifi_data[1]=='0'		)//关灯
					  LED1=1;
					  else	if( Recwifi_data[0]=='1'&&   Recwifi_data[1]=='1'		)//开灯
					  LED1=0 ;
				   	  if( Recwifi_data[0]=='2'&&   Recwifi_data[1]=='0'		)//关灯
					  LED2=1;
					  else	if( Recwifi_data[0]=='2'&&   Recwifi_data[1]=='1'		)//开灯
					  LED2=0 ;
					 Clear_Buffer();	
                 }
					
    }
}
void Send_DATA(uchar *buffer)
{
uchar i;
     	Send_Str("AT+CIPSEND=0,7\r\n");
		delayms(300);
    	Send_Str(buffer);//发送数据
    	delayms(100);
		strx=strstr((const char*)Rxbuff,(const char*)"SEND OK");//返回OK
		while(strx==NULL)
		{
				delayms(100);
				strx=strstr((const char*)Rxbuff,(const char*)"SEND OK");//返回OK
		}
	 	strx=strstr((const char*)Rxbuff,(const char*)"+IPD");//有数据返回	 ,顺便判断是否有数据返回
		  if(strx)//正常获取数据
			 {
				 clinetid=strx[5];
				 strx=strstr((const char*)Rxbuff,(const char*)":");//有数据返回
				 for(i=0;i<5;i++)
				 Recwifi_data[i]=strx[1+i];		  //获取上位机设定的报警值
				 Clear_Buffer();	
             }
			Clear_Buffer();	 
}
 void main()
 {
  unsigned char   Tx_Buf[10];
  uchar i;
  unsigned char LEDstatus;//灯的状态
  	delayms(500);
	delayms(500);
	delayms(1000);	  //延时一段时间，让WIFI模块稳定
 	InitUART();			//初始化串口
    ESP8266_SERVER();	//初始化ESP8266
 	while(1)	
	{	  
	   Get_Clinet();
		if(getflag)
	{
		  getDHT11(); //获取温湿度值
	  Tx_Buf[0]='T';	//帧头
	  Tx_Buf[1]=F16T/10%10+0x30;		 //将温湿度数据送往发送数组,送给蓝牙模块让手机APP显示
	  Tx_Buf[2]=F16T%10+0x30;		
	  Tx_Buf[3]=F16RH/10%10+0x30;
	  Tx_Buf[4]=F16RH%10+0x30;
		LEDstatus=LED1;
	   Tx_Buf[5]=LEDstatus+0x30;//发送灯的状态
		LEDstatus=LED2;
		Tx_Buf[6]=LEDstatus+0x30;
	   Send_DATA(Tx_Buf)	;//发送数据

	
	}
	}
 }

/*****************串口接收中断函数,接收蓝牙模块的数据*********************/
void UARTInterrupt(void) interrupt 4
{	
if(RI)
    {
		ES=0;
        RI = 0;
      Rxbuff[Rxnum++]=SBUF;
	if(Rxnum>=50)
		Rxnum=0;
	    ES=1;
    }
  
}
 