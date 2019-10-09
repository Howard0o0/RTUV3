//////////////////////////////////////////////////////
//     �ļ���: uart1.c
//   �ļ��汾: 1.0.0
//   ����ʱ��: 09��11��30��
//   ��������:  
//       ����: ����
//       ��ע: ��
//
//////////////////////////////////////////////////////

#include "msp430common.h"
#include "common.h"
#include "uart1.h"
#include "uart0.h"
#include "uart2.h"
#include "uart3.h"
#include "led.h"
#include "stdio.h"
/************A1********/
#define TXD2 BIT4
#define RXD2 BIT5

//char testbuf[2];
//int testcount=0;

char * UART2_Tx_Buf=NULL; 
char UART2_Rx_Buffer[UART2_MAXIndex][UART2_MAXBUFFLEN]; //  ���ݴ洢�� 
int  UART2_Rx_BufLen[UART2_MAXIndex];                   //  ÿ�н��յ������ݳ���  
int  UART2_Rx_INTIndex=0;                               //  �жϸ�д����λ��
int  UART2_Rx_INTLen=0;                                 //  �жϸ�д���еĵڼ����ַ� 
int  UART2_Rx_RecvIndex=0;                              //  ��ǰ�ö�����λ�� 

unsigned int UART2_Tx_Flag=0;
unsigned int UART2_Tx_Len=0;
extern int WIFI_Inited_Flag;


//ָʾ��ǰ���͵�
static int s_uart2_type=0;  

int UART2_Open(int  _type)
{
    s_uart2_type = _type;
 

 
  UCA2CTL1 |= UCSWRST;
  UCA2CTL1 |= UCSSEL1;   //smclk 1M 
  
  UCA2BR0 = 8;
  UCA2BR1 = 0;
  UCA2MCTL |= UCBRF_0+UCBRS_6;
  UCA2CTL1 &= ~UCSWRST;
  
  UART2_ClearBuffer();
  /********************/
  P9DIR |= TXD2;
  P9SEL |= TXD2 + RXD2;
  
  /*2418 UC1IE UCA1RXIE 5438 UCA1IE UCRXIE*/
  UCA2IE |= UCRXIE;//���ܴ��ڽ����ж�          

  return 0;
}
void UART2_Open_9600(int _type)
{
  s_uart2_type = _type;
    
    //����rs232  ��ƽת����·
 //   P4DIR |= BIT0;
 //   P4OUT |= BIT0;
    
 //   UCTL1 = 0x00;
 //   UCTL1 &=~ SWRST; 
 //   UCTL1 |= CHAR;
 //   UTCTL1 = 0X00;	
    //115200��  XT2=8000000   SMCLK
    //UTCTL1=SSEL1; UBR0_1 = 0x45; UBR1_1 = 0x00; UMCTL1 = 0x4A;
    
    //  9600,    XT2=8000000   SMCLK
 //   UTCTL1=SSEL1;UBR0_1 = 0x41;UBR1_1 = 0x03;UMCTL1 = 0x00;
     
  //  UART1_ClearBuffer();
        
  //  ME2 |= UTXE1+URXE1;   //ʹ��UART1��TXD��RXD  
 //   IE2 |= URXIE1+UTXIE1; //ʹ��UART1��RX��TX�ж�  
    
 //   P3SEL |= BIT6;//����P3.6ΪUART1��TXD 
 //   P3SEL |= BIT7;//����P3.7ΪUART1��RXD
 //   P3DIR |= BIT6;//P3.6Ϊ����ܽ�    
  UCA2CTL1 |= UCSWRST;
  UCA2CTL1 |= UCSSEL1;   //smclk 1M 
  
  //104��Ӧ9600�����ʣ�52��Ӧ19200? 8��Ӧ115200
  UCA2BR0 = 104;
  UCA2BR1 = 0;
  UCA2MCTL |= UCBRF_0+UCBRS_6;//
  //UCA1MCTL = 10;
  UCA2CTL1 &= ~UCSWRST;
  
  UART2_ClearBuffer();
  /********************/
  P9DIR |= TXD2;
  P9SEL |= TXD2 + RXD2;
  
  /*2418 UC1IE UCA1RXIE 5438 UCA1IE UCRXIE*/
  UCA2IE |= UCRXIE;
}
void UART2_Close()
{ 
   //�ر�RS232��ƽת����·
  //P4DIR |= BIT0;
  //P4OUT &= ~BIT0; 
   
   UART2_ClearBuffer(); 
   //�رմ���1
   
/*2418 UC1IE UCA1RXIE 5438 UCA1IE UCRXIE*/
   UCA2IE &= ~UCRXIE;	 

}

void UART2_ClearBuffer()
{
    DownInt();//���ж�
    
    UART2_Tx_Buf=0;
    UART2_Rx_INTIndex=0;
    UART2_Rx_INTLen=0;
    UART2_Rx_RecvIndex=0;
  
    UART2_Tx_Flag=0;
    UART2_Tx_Len=0; 
    
    for(int i=0;i<UART2_MAXIndex;++i){
      UART2_Rx_BufLen[i]=0;
    }
    
    UpInt();//���ж�
}
int  UART2_Send(char * _data ,int _len, int _CR)
{

    if(UART2_Tx_Flag!=0)//�ȴ���һ�η��ͽ���
    {//�͵�500ms 
        System_Delayms(500);
   
        UART2_Tx_Flag=0;//ǿ������Ϊ0;
    }
    if(_len>1)
    {
        //��ȫ�ֱ�����ֵ
        UART2_Tx_Buf=_data; //�����һ�������ж� ����Ϊ0
        UART2_Tx_Len=_len; //�����һ�������ж� ����Ϊ0,�෢���һ����������
        UART2_Tx_Flag=1; //���������������һ�����ݵ��ж���������Ϊ0��  
       for(int i=0;i<UART2_Tx_Len;i++)
       {
         
         /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
            UCA2TXBUF =_data[i];
       }
        UART2_Tx_Flag=0;
    }
    if(_len==1)//1���ַ���ʱ�� ���жϷ��� �޷��ɹ�.��ʱ�Ȼ��ɲ�ѯ����.�Ժ���о�
    {
      /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
        UCA2TXBUF = _data[0];
        
    }
    if(_CR)
    {//����һ������
      /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
        UCA2TXBUF=13;
        /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
        UCA2TXBUF=10;
        
    }
    if(_CR)
    {//����һ������
      /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
        UCA2TXBUF=13;
        /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
        UCA2TXBUF=10;
        
    }
    return 0;
}

//����
int  UART2_SendtoInt(int num)
{
/*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
            UCA2TXBUF =num;
          System_Delayms(2000);
    return 0;
}

int  UART2_RecvLine(char * _dest ,int _max, int * _pNum)
{
    int i=0;
    //�ö���λ�ó���Ϊ0, ��ѭ���ȴ� 
    while(UART2_Rx_BufLen[UART2_Rx_RecvIndex]==0);
    //�������ˣ��Ͱ����ݸ��Ƴ���, 
    for(i=0; ( i< _max) && ( i<UART2_Rx_BufLen[UART2_Rx_RecvIndex]); ++i)
    {
        _dest[i]=UART2_Rx_Buffer[UART2_Rx_RecvIndex][i];
    }
    *_pNum = UART2_Rx_BufLen[UART2_Rx_RecvIndex];
    //������Ϻ�,�ͰѸ�λ�õĳ�������Ϊ0,�����жϿ��Ը�����.
    UART2_Rx_BufLen[UART2_Rx_RecvIndex]=0;
    //��λ����һ��
    // ��� ����9 �ͼ�ȥ9 ,�ӵ�һ�п�ʼ,����ͼ�������.
    if( UART2_Rx_RecvIndex >= UART2_MAXIndex -1)
        UART2_Rx_RecvIndex=0;
    else
        ++UART2_Rx_RecvIndex;
    return 0;
}

int  UART2_RecvLineTry(char * _dest,const int _max, int * _pNum)
{
    int i=0;
    //�ö���λ�ó���Ϊ0, ��ѭ���ȴ� 

    if(UART2_Rx_BufLen[UART2_Rx_RecvIndex]==0)
    {
        return -1;
    }

    TraceInt4(UART2_Rx_RecvIndex,1);
    TraceInt4(UART2_Rx_BufLen[UART2_Rx_RecvIndex],1);
    //�������ˣ��Ͱ����ݸ��Ƴ���, 
    for(i=0; ( i< _max) && ( i<UART2_Rx_BufLen[UART2_Rx_RecvIndex]); ++i)
    {
        _dest[i]=UART2_Rx_Buffer[UART2_Rx_RecvIndex][i];
    }
    *_pNum =UART2_Rx_BufLen[UART2_Rx_RecvIndex];
    //������Ϻ�,�ͰѸ�λ�õĳ�������Ϊ0,�����жϿ��Ը�����.
    UART2_Rx_BufLen[UART2_Rx_RecvIndex]=0;
    //��λ����һ��
    // ��� ����9 �ͼ�ȥ9 ,�ӵ�һ�п�ʼ,����ͼ�������.
    if( UART2_Rx_RecvIndex >= UART2_MAXIndex -1)
        UART2_Rx_RecvIndex=0;
    else
        ++UART2_Rx_RecvIndex;
    return 0;
}
int  UART2_RecvLineWait(char *_dest ,const int _max, int * _pNum)
{
    int i=0; 
    //�ö���λ�ó���Ϊ0, ��ѭ���ȴ� 
    while(UART2_Rx_BufLen[UART2_Rx_RecvIndex]==0)
    {
        System_Delayms(30);
        ++i;
        if(i>10) 
            return -1;
    }
    //�������ˣ��Ͱ����ݸ��Ƴ���, 
    for(i=0; ( i< _max) && ( i<UART2_Rx_BufLen[UART2_Rx_RecvIndex]); ++i)
    {
        _dest[i]=UART2_Rx_Buffer[UART2_Rx_RecvIndex][i];
    }
    *_pNum = UART2_Rx_BufLen[UART2_Rx_RecvIndex];
    //������Ϻ�,�ͰѸ�λ�õĳ�������Ϊ0,�����жϿ��Ը�����.
    UART2_Rx_BufLen[UART2_Rx_RecvIndex]=0;
    //��λ����һ��
    // ��� ����9 �ͼ�ȥ9 ,�ӵ�һ�п�ʼ,����ͼ�������.
    if( UART2_Rx_RecvIndex >= UART2_MAXIndex -1)
        UART2_Rx_RecvIndex=0;
    else
        ++UART2_Rx_RecvIndex;
    return 0;
}
int  UART2_RecvLineLongWait(char *_dest,int _max, int * _pNum)
{
    int i=0;
    //�ö���λ�ó���Ϊ0, ��ѭ���ȴ� 
    while(UART2_Rx_BufLen[UART2_Rx_RecvIndex]==0)
    {//�ȴ�5��.
        System_Delayms(50);
        ++i;
        if(i>100)
            return -1;        
    }
    //�������ˣ��Ͱ����ݸ��Ƴ���, 
    for(i=0; ( i< _max) && ( i<UART2_Rx_BufLen[UART2_Rx_RecvIndex]); ++i)
    {
        _dest[i]=UART2_Rx_Buffer[UART2_Rx_RecvIndex][i];
    }
    *_pNum = UART2_Rx_BufLen[UART2_Rx_RecvIndex];
    //������Ϻ�,�ͰѸ�λ�õĳ�������Ϊ0,�����жϿ��Ը�����.
    UART2_Rx_BufLen[UART2_Rx_RecvIndex]=0;
    //��λ����һ��
    // ��� ����9 �ͼ�ȥ9 ,�ӵ�һ�п�ʼ,����ͼ�������.
    if( UART2_Rx_RecvIndex >= UART2_MAXIndex -1)
        UART2_Rx_RecvIndex=0;
    else
        ++UART2_Rx_RecvIndex;
    return 0;
}

//void Judge_Watermeter()
//{
//    if(UART2_Rx_INTLen == 9)
//    {
//        UART2_Rx_BufLen[UART2_Rx_INTIndex] = UART2_Rx_INTLen;
//        UART2_Rx_INTLen=0;
//        if(UART2_Rx_INTIndex >= UART2_MAXIndex-1)
//            UART2_Rx_INTIndex=0;
//        else
//            ++UART2_Rx_INTIndex;
//    }
//}


//
//    ���ܵ�һ���ַ�.
//    
//    װ�ص�UART2_Rx_Buffer[UART2_Rx_INTIndex][UART2_Rx_INTLen]��.
//    ������UART2_Rx_INTLen ;���UART2_Rx_INTLen�Ѿ������һ�������ַ�Ϊ����
//    ��д���л���ĳ���Ϊ UART2_Rx_INTLen+1;
//    ����UART2_Rx_INTIndex,ָ����һ��������. �����һ����������������δ������.
//    ��ô�Ͳ�����,������д��ǰ�Ļ�����.
//    
//    
//    
/*************VECTOR*/
#pragma vector=USCI_A2_VECTOR 
__interrupt void UART2_RX_ISR(void)   //�����յ����ַ���ʾ���������
{
   //_DINT();
   char _temp; 
      //char *tbuffer;  
   _temp = UCA2RXBUF;
#if 1

    UART2_Rx_Buffer[UART2_Rx_INTIndex][UART2_Rx_INTLen]=_temp;
    ++UART2_Rx_INTLen;
    
    if(s_uart2_type == UART2_CONSOLE_TYPE)
    {   
      
        if(((_temp==0x0A) && (UART2_Rx_INTLen!=0) && (UART2_Rx_Buffer[UART2_Rx_INTIndex][UART2_Rx_INTLen-2]==0x0D)) || (_temp == ')'))
        {
            //�����ͷ���յ���������з���,ֱ������
            if(UART2_Rx_INTLen==1)
            {
                UART2_Rx_INTLen=0; //���¿�ʼ���� 
                return ;
            }
            else
            {
                //   ��λ����һ�� 
                //UART2_Rx_Buffer[UART2_Rx_INTIndex][UART2_Rx_INTLen-1]=13; //������ø�13
                UART2_Rx_BufLen[UART2_Rx_INTIndex] = UART2_Rx_INTLen - 2;//���������з�
                UART2_Rx_INTLen=0;
                if(UART2_Rx_INTIndex >= UART2_MAXIndex-1)
                    UART2_Rx_INTIndex=0;
                else
                    ++UART2_Rx_INTIndex;
                //UART2_Rx_INTIndex += UART2_Rx_INTIndex < (UART2_MAXIndex - 1) ? 1 : 1-UART2_MAXIndex;  
                return ;
            }
        }
    }
    else
    {
        Judge_Watermeter();
        return ;
    }
    
//    if(_temp==0x0A)
//        return ;
//
//    if((_temp==0x0D) && (UART2_Rx_INTLen!=0) && UART2_Rx_Buffer[UART2_Rx_INTIndex][UART2_Rx_INTLen-1]==0x0D)
//    {
//        return;
//    }
    
    if(UART2_Rx_INTLen >= UART2_MAXBUFFLEN-1)
    {//�г���������, ����ֱ�ӽضϳ�һ��.
        UART2_Rx_BufLen[UART2_Rx_INTIndex] = UART2_Rx_INTLen + 1;
        UART2_Rx_INTLen=0;
        if(UART2_Rx_INTIndex >= UART2_MAXIndex-1)
            UART2_Rx_INTIndex=0;
        else
            ++UART2_Rx_INTIndex;
    }
    
    //�жϻ������Ƿ�����:UART2_Rx_INTIndex��¼�´α�����������´������Ѿ��д洢��˵������������
    if( UART2_Rx_BufLen[UART2_Rx_INTIndex]!=0)
    {
        //��һ�л�δ�������Ǿ͸�����������һ��
        if(UART2_Rx_INTIndex <= 0)
            UART2_Rx_INTIndex = UART2_MAXIndex-1;
        else
            --UART2_Rx_INTIndex;
        //�Ѹ��г�������Ϊ0,���ж�ռ��
        UART2_Rx_BufLen[UART2_Rx_INTIndex]=0;
    }

 #endif
}

//
//int putchar(int c)
//{
//    while (!(UCA2IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
//    UCA2TXBUF = (unsigned char)c; 
//  
//    return c;
//}