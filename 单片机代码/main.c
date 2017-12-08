 #include "DHT11.h" 
 #include <stdio.h>
 #include "string.h"
 extern unsigned char  F16T,F16RH;		//��ʪ������
sbit LED1=P3^7;
sbit LED2=P3^6;
#define uchar unsigned char 
#define uint unsigned int
unsigned char idata Rxbuff[50],Rxnum;
unsigned char	Recwifi_data[5];
char *strx=0;
char *strx1=0;
char clinetid;//����ID
unsigned char getflag;//��ȡ��־
void Get_Clinet(void);
void InitUART(void)			 //���Ǵ��ڵĻ������ã��������Ĳ�������9600.��Щ�������Ǳ�׼�ġ�
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
 void Clear_Buffer(void)//��ջ���
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
 * UART ����һ�ֽ�
*/
void UART_send_byte(char dat)
{
	SBUF = dat;
	while (TI == 0);
	TI = 0;
}

/*
 * UART �����ַ���
*/
void Send_Str(unsigned char *buf)
{
	while (*buf != '\0')
	{
		UART_send_byte(*buf++);
	}
}

void ESP8266_SERVER(void)//����������
{

	Send_Str("AT\r\n");
	delayms(500);
	Send_Str("AT\r\n");
	delayms(500);
	Send_Str("AT\r\n");
	delayms(500);
	Send_Str("ATE0\r\n");	  //�رջ���
	delayms(500);
   	Send_Str("AT+CWSAP=\042ESP8266_001\042,\0421234567890\042,5,3\r\n");	  //������ʾ���ƺ�����
	delayms(500);
	Send_Str("AT+CIPMUX=1\r\n");//���������ӣ���������������Ҫ����
	delayms(500);	
	Clear_Buffer();	
	Send_Str("AT+CIPSERVER=1,8888\r\n");//����������
	delayms(500);	
	strx=strstr((const char*)Rxbuff,(const char*)"OK");//�ȴ��������
	while(strx==NULL)
	{		Clear_Buffer();	
			delayms(500);
			strx=strstr((const char*)Rxbuff,(const char*)"OK");//�ȴ��������
	}
	Clear_Buffer();	

}
void Get_Clinet(void)//�ж��Ƿ��ȡ���˿ͻ��˽���
{

	   uchar i;
     
		strx=strstr((const char*)Rxbuff,(const char*)"CONNECT");//��������ֵ
			if(strx)
		{
		  Clear_Buffer();	
			getflag=1;
		}
		strx=strstr((const char*)Rxbuff,(const char*)"CLOSE");//���ضϿ�����
			if(strx)
		{
		  Clear_Buffer();	
			getflag=0;
		}
		if(getflag)
		{
				strx=strstr((const char*)Rxbuff,(const char*)"+IPD");//�����ݷ���
			  if(strx)//������ȡ����
				 {
					 clinetid=strx[5];
					 strx=strstr((const char*)Rxbuff,(const char*)":");//�����ݷ���
					 for(i=0;i<2;i++)
					 Recwifi_data[i]=strx[1+i];		  //��ȡ�ֻ���LED�Ŀ���
					  if( Recwifi_data[0]=='1'&&   Recwifi_data[1]=='0'		)//�ص�
					  LED1=1;
					  else	if( Recwifi_data[0]=='1'&&   Recwifi_data[1]=='1'		)//����
					  LED1=0 ;
				   	  if( Recwifi_data[0]=='2'&&   Recwifi_data[1]=='0'		)//�ص�
					  LED2=1;
					  else	if( Recwifi_data[0]=='2'&&   Recwifi_data[1]=='1'		)//����
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
    	Send_Str(buffer);//��������
    	delayms(100);
		strx=strstr((const char*)Rxbuff,(const char*)"SEND OK");//����OK
		while(strx==NULL)
		{
				delayms(100);
				strx=strstr((const char*)Rxbuff,(const char*)"SEND OK");//����OK
		}
	 	strx=strstr((const char*)Rxbuff,(const char*)"+IPD");//�����ݷ���	 ,˳���ж��Ƿ������ݷ���
		  if(strx)//������ȡ����
			 {
				 clinetid=strx[5];
				 strx=strstr((const char*)Rxbuff,(const char*)":");//�����ݷ���
				 for(i=0;i<5;i++)
				 Recwifi_data[i]=strx[1+i];		  //��ȡ��λ���趨�ı���ֵ
				 Clear_Buffer();	
             }
			Clear_Buffer();	 
}
 void main()
 {
  unsigned char   Tx_Buf[10];
  uchar i;
  unsigned char LEDstatus;//�Ƶ�״̬
  	delayms(500);
	delayms(500);
	delayms(1000);	  //��ʱһ��ʱ�䣬��WIFIģ���ȶ�
 	InitUART();			//��ʼ������
    ESP8266_SERVER();	//��ʼ��ESP8266
 	while(1)	
	{	  
	   Get_Clinet();
		if(getflag)
	{
		  getDHT11(); //��ȡ��ʪ��ֵ
	  Tx_Buf[0]='T';	//֡ͷ
	  Tx_Buf[1]=F16T/10%10+0x30;		 //����ʪ������������������,�͸�����ģ�����ֻ�APP��ʾ
	  Tx_Buf[2]=F16T%10+0x30;		
	  Tx_Buf[3]=F16RH/10%10+0x30;
	  Tx_Buf[4]=F16RH%10+0x30;
		LEDstatus=LED1;
	   Tx_Buf[5]=LEDstatus+0x30;//���͵Ƶ�״̬
		LEDstatus=LED2;
		Tx_Buf[6]=LEDstatus+0x30;
	   Send_DATA(Tx_Buf)	;//��������

	
	}
	}
 }

/*****************���ڽ����жϺ���,��������ģ�������*********************/
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
 