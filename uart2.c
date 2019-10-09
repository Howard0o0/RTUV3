//////////////////////////////////////////////////////
//     文件名: uart1.c
//   文件版本: 1.0.0
//   创建时间: 09年11月30日
//   更新内容:  
//       作者: 林智
//       附注: 无
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
char UART2_Rx_Buffer[UART2_MAXIndex][UART2_MAXBUFFLEN]; //  数据存储区 
int  UART2_Rx_BufLen[UART2_MAXIndex];                   //  每行接收到的数据长度  
int  UART2_Rx_INTIndex=0;                               //  中断该写的行位置
int  UART2_Rx_INTLen=0;                                 //  中断该写该行的第几个字符 
int  UART2_Rx_RecvIndex=0;                              //  当前该读的行位置 

unsigned int UART2_Tx_Flag=0;
unsigned int UART2_Tx_Len=0;
extern int WIFI_Inited_Flag;


//指示当前类型的
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
  UCA2IE |= UCRXIE;//是能串口接收中断          

  return 0;
}
void UART2_Open_9600(int _type)
{
  s_uart2_type = _type;
    
    //开启rs232  电平转换电路
 //   P4DIR |= BIT0;
 //   P4OUT |= BIT0;
    
 //   UCTL1 = 0x00;
 //   UCTL1 &=~ SWRST; 
 //   UCTL1 |= CHAR;
 //   UTCTL1 = 0X00;	
    //115200，  XT2=8000000   SMCLK
    //UTCTL1=SSEL1; UBR0_1 = 0x45; UBR1_1 = 0x00; UMCTL1 = 0x4A;
    
    //  9600,    XT2=8000000   SMCLK
 //   UTCTL1=SSEL1;UBR0_1 = 0x41;UBR1_1 = 0x03;UMCTL1 = 0x00;
     
  //  UART1_ClearBuffer();
        
  //  ME2 |= UTXE1+URXE1;   //使能UART1的TXD和RXD  
 //   IE2 |= URXIE1+UTXIE1; //使能UART1的RX和TX中断  
    
 //   P3SEL |= BIT6;//设置P3.6为UART1的TXD 
 //   P3SEL |= BIT7;//设置P3.7为UART1的RXD
 //   P3DIR |= BIT6;//P3.6为输出管脚    
  UCA2CTL1 |= UCSWRST;
  UCA2CTL1 |= UCSSEL1;   //smclk 1M 
  
  //104对应9600波特率，52对应19200? 8对应115200
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
   //关闭RS232电平转换电路
  //P4DIR |= BIT0;
  //P4OUT &= ~BIT0; 
   
   UART2_ClearBuffer(); 
   //关闭串口1
   
/*2418 UC1IE UCA1RXIE 5438 UCA1IE UCRXIE*/
   UCA2IE &= ~UCRXIE;	 

}

void UART2_ClearBuffer()
{
    DownInt();//关中断
    
    UART2_Tx_Buf=0;
    UART2_Rx_INTIndex=0;
    UART2_Rx_INTLen=0;
    UART2_Rx_RecvIndex=0;
  
    UART2_Tx_Flag=0;
    UART2_Tx_Len=0; 
    
    for(int i=0;i<UART2_MAXIndex;++i){
      UART2_Rx_BufLen[i]=0;
    }
    
    UpInt();//开中断
}
int  UART2_Send(char * _data ,int _len, int _CR)
{

    if(UART2_Tx_Flag!=0)//等待上一次发送结束
    {//就等500ms 
        System_Delayms(500);
   
        UART2_Tx_Flag=0;//强制设置为0;
    }
    if(_len>1)
    {
        //给全局变量赋值
        UART2_Tx_Buf=_data; //由最后一次数据中断 设置为0
        UART2_Tx_Len=_len; //由最后一次数据中断 设置为0,多发最后一个结束符号
        UART2_Tx_Flag=1; //这个变量最后由最后一个数据的中断重新设置为0；  
       for(int i=0;i<UART2_Tx_Len;i++)
       {
         
         /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
            UCA2TXBUF =_data[i];
       }
        UART2_Tx_Flag=0;
    }
    if(_len==1)//1个字符的时候 用中断发送 无法成功.暂时先换成查询发送.以后待研究
    {
      /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
        UCA2TXBUF = _data[0];
        
    }
    if(_CR)
    {//补发一个换行
      /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
        UCA2TXBUF=13;
        /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
        UCA2TXBUF=10;
        
    }
    if(_CR)
    {//补发一个换行
      /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
        UCA2TXBUF=13;
        /*2418 UC1IFG UCA1TXIFG 5438 UCA2IFG UCTXIFG*/
        while (!(UCA2IFG&UCTXIFG));
        UCA2TXBUF=10;
        
    }
    return 0;
}

//调试
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
    //该读的位置长度为0, 则循环等待 
    while(UART2_Rx_BufLen[UART2_Rx_RecvIndex]==0);
    //有数据了，就把数据复制出来, 
    for(i=0; ( i< _max) && ( i<UART2_Rx_BufLen[UART2_Rx_RecvIndex]); ++i)
    {
        _dest[i]=UART2_Rx_Buffer[UART2_Rx_RecvIndex][i];
    }
    *_pNum = UART2_Rx_BufLen[UART2_Rx_RecvIndex];
    //复制完毕后,就把该位置的长度设置为0,告诉中断可以覆盖了.
    UART2_Rx_BufLen[UART2_Rx_RecvIndex]=0;
    //定位到下一行
    // 如果 等于9 就减去9 ,从第一行开始,否则就继续自增.
    if( UART2_Rx_RecvIndex >= UART2_MAXIndex -1)
        UART2_Rx_RecvIndex=0;
    else
        ++UART2_Rx_RecvIndex;
    return 0;
}

int  UART2_RecvLineTry(char * _dest,const int _max, int * _pNum)
{
    int i=0;
    //该读的位置长度为0, 则循环等待 

    if(UART2_Rx_BufLen[UART2_Rx_RecvIndex]==0)
    {
        return -1;
    }

    TraceInt4(UART2_Rx_RecvIndex,1);
    TraceInt4(UART2_Rx_BufLen[UART2_Rx_RecvIndex],1);
    //有数据了，就把数据复制出来, 
    for(i=0; ( i< _max) && ( i<UART2_Rx_BufLen[UART2_Rx_RecvIndex]); ++i)
    {
        _dest[i]=UART2_Rx_Buffer[UART2_Rx_RecvIndex][i];
    }
    *_pNum =UART2_Rx_BufLen[UART2_Rx_RecvIndex];
    //复制完毕后,就把该位置的长度设置为0,告诉中断可以覆盖了.
    UART2_Rx_BufLen[UART2_Rx_RecvIndex]=0;
    //定位到下一行
    // 如果 等于9 就减去9 ,从第一行开始,否则就继续自增.
    if( UART2_Rx_RecvIndex >= UART2_MAXIndex -1)
        UART2_Rx_RecvIndex=0;
    else
        ++UART2_Rx_RecvIndex;
    return 0;
}
int  UART2_RecvLineWait(char *_dest ,const int _max, int * _pNum)
{
    int i=0; 
    //该读的位置长度为0, 则循环等待 
    while(UART2_Rx_BufLen[UART2_Rx_RecvIndex]==0)
    {
        System_Delayms(30);
        ++i;
        if(i>10) 
            return -1;
    }
    //有数据了，就把数据复制出来, 
    for(i=0; ( i< _max) && ( i<UART2_Rx_BufLen[UART2_Rx_RecvIndex]); ++i)
    {
        _dest[i]=UART2_Rx_Buffer[UART2_Rx_RecvIndex][i];
    }
    *_pNum = UART2_Rx_BufLen[UART2_Rx_RecvIndex];
    //复制完毕后,就把该位置的长度设置为0,告诉中断可以覆盖了.
    UART2_Rx_BufLen[UART2_Rx_RecvIndex]=0;
    //定位到下一行
    // 如果 等于9 就减去9 ,从第一行开始,否则就继续自增.
    if( UART2_Rx_RecvIndex >= UART2_MAXIndex -1)
        UART2_Rx_RecvIndex=0;
    else
        ++UART2_Rx_RecvIndex;
    return 0;
}
int  UART2_RecvLineLongWait(char *_dest,int _max, int * _pNum)
{
    int i=0;
    //该读的位置长度为0, 则循环等待 
    while(UART2_Rx_BufLen[UART2_Rx_RecvIndex]==0)
    {//等待5秒.
        System_Delayms(50);
        ++i;
        if(i>100)
            return -1;        
    }
    //有数据了，就把数据复制出来, 
    for(i=0; ( i< _max) && ( i<UART2_Rx_BufLen[UART2_Rx_RecvIndex]); ++i)
    {
        _dest[i]=UART2_Rx_Buffer[UART2_Rx_RecvIndex][i];
    }
    *_pNum = UART2_Rx_BufLen[UART2_Rx_RecvIndex];
    //复制完毕后,就把该位置的长度设置为0,告诉中断可以覆盖了.
    UART2_Rx_BufLen[UART2_Rx_RecvIndex]=0;
    //定位到下一行
    // 如果 等于9 就减去9 ,从第一行开始,否则就继续自增.
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
//    接受到一个字符.
//    
//    装载到UART2_Rx_Buffer[UART2_Rx_INTIndex][UART2_Rx_INTLen]中.
//    并递增UART2_Rx_INTLen ;如果UART2_Rx_INTLen已经是最后一个或者字符为换行
//    填写本行缓存的长度为 UART2_Rx_INTLen+1;
//    递增UART2_Rx_INTIndex,指向下一个缓冲区. 如果下一个缓存区的数据仍未被处理.
//    那么就不递增,覆盖填写当前的缓冲区.
//    
//    
//    
/*************VECTOR*/
#pragma vector=USCI_A2_VECTOR 
__interrupt void UART2_RX_ISR(void)   //将接收到的字符显示到串口输出
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
            //如果是头部收到的这个换行符号,直接抛弃
            if(UART2_Rx_INTLen==1)
            {
                UART2_Rx_INTLen=0; //重新开始接收 
                return ;
            }
            else
            {
                //   定位到下一行 
                //UART2_Rx_Buffer[UART2_Rx_INTIndex][UART2_Rx_INTLen-1]=13; //随便设置个13
                UART2_Rx_BufLen[UART2_Rx_INTIndex] = UART2_Rx_INTLen - 2;//不包括换行符
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
    {//行长度满出了, 我们直接截断成一行.
        UART2_Rx_BufLen[UART2_Rx_INTIndex] = UART2_Rx_INTLen + 1;
        UART2_Rx_INTLen=0;
        if(UART2_Rx_INTIndex >= UART2_MAXIndex-1)
            UART2_Rx_INTIndex=0;
        else
            ++UART2_Rx_INTIndex;
    }
    
    //判断缓冲区是否已满:UART2_Rx_INTIndex记录下次保存的行数，下次行数已经有存储，说明缓冲器满了
    if( UART2_Rx_BufLen[UART2_Rx_INTIndex]!=0)
    {
        //下一行还未被处理，那就覆盖最后处理的这一行
        if(UART2_Rx_INTIndex <= 0)
            UART2_Rx_INTIndex = UART2_MAXIndex-1;
        else
            --UART2_Rx_INTIndex;
        //把该行长度设置为0,由中断占用
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