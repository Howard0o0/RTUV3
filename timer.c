//////////////////////////////////////////////////////
//     文件名: timer.c
//   文件版本: 1.0.0  
//   创建时间: 09年11月30日
//   更新内容: 无。 
//       作者: 林智
//       附注:  
// 
//
//////////////////////////////////////////////////////

#include "msp430common.h"
#include "timer.h"
#include "common.h"
#include "led.h"
#include "hydrologytask.h"

static unsigned int s_ClockTicks=0;   
static unsigned int s_reset_count=0;
void Timer_Zero()
{
    s_ClockTicks=0;
} 
int Timer_Passed_Mins(int _min)
{
    if(_min <= s_ClockTicks)
        return 1;
    else
        return 0;
}
/*time1 a3*/
//注意 ACLK 使用了 8分频,
void TimerA_Init(unsigned int ccr0)
{
  /*2418 TACTL 5438 TA1CTL*/
    TA1CTL =  TACLR;      //清除计数
    TA1CTL |= TASSEL0;    //选择 ACLK时钟
    TA1CTL |= ID1+ID0;    // 1/8分频 
    /*2418 TACCTL0 5438 TA1CCTL0*/
    TA1CCTL0 = CCIE;	 // CCR0 中断允许
    //1分钟 中断一次
    /*2418 TACCR0 5438 TA1CCR0*/
    TA1CCR0 = ccr0;  //  (32768HZ / (8 * 8) ) * 60s =30720
    /*2418 TACTL 5438 TA1CTL*/
    TA1CTL |= MC0 ;// 增计数 模式   
} 
 
void WatchDog_Init()
{ 
    //开成最大情况的.
    //使用ACLK 32K/8 分频
  /*2418 WDTSSEL 5438 WDTSSEL0*/
    WDTCTL = WDTPW + WDTSSEL0; 
}  
void WatchDog_Clear()
{
  /*2418 WDTSSEL 5438 WDTSSEL0*/
    WDTCTL = WDTPW  + WDTSSEL0 + WDTCNTCL;
} 
/*time0 b7*/
void TimerB_Init(unsigned int ccr0)
{
  /*2418 TBCTL 5438 TB0CTL*/
    TB0CTL = TBCLR;
    TB0CTL |= TBSSEL0;    //选择ACLK时钟
    TB0CTL |= ID0;  //1/8分频
    TB0CTL |= MC1  ;   //   连续计数模式          
    //TBCTL |= TBIE;       //使能TBIFG中断
    //4分钟 中断一次 ,(跑2趟)
    //TBCCR0=ccr0; // (32768HZ /(8*8) ) * 240 / 2 = 61440
    TBCCR1=61440; 
//    TBCCR2=10240;
//    TBCCR2=4096;//用来清狗的 中断. 2s中断1次
//    TBCCR2=2048;
    TBCCR2=1024;//用来清狗的 中断.
    /*2418 TBCCTL1 TBCCTL2 5438 TB0CCTL1 TB0CCTL2*/
    TB0CCTL1=CCIE;
    TB0CCTL2=CCIE;
    /*2418 P41  5438 P96 systemdebug*/
    P9DIR |= BIT6;
    P9OUT |= BIT6;
}

void TimerB_Clear()
{
    s_reset_count=0;
}
/*   TIMERA0_VECTOR   *   TIMER1_A0_VECTOR*/
#pragma vector=TIMER1_A0_VECTOR 
__interrupt void TIMERA0_ISR(void)
{
    //唤醒CPU,如果CPU本来就是醒的,不会导致混乱
    //LPM3_EXIT;
    LPM2_EXIT;
   // Led1_On();
 //   System_Delayms(75);
   // Led1_Off();
    ++s_ClockTicks;//增加一次滴答 
    HydrologyTimeBase();
}

//12分钟的看门狗
#pragma vector = TIMERB0_VECTOR
__interrupt void TIMERB0_ISR (void)
{
//    System_Delayms(1000);
}

#pragma vector=TIMERB1_VECTOR
__interrupt void TIMERB1_ISR(void)
{ 
    //必须要读一下TBIV来清中断
    unsigned int _tbiv = TBIV ; 
    switch(_tbiv)
    {
      case 2:
        //半分钟来一次
        //计数如果超过1000
        ++s_reset_count;
        if(s_reset_count>60)
        {//超过12分钟就重启
            System_Reset();
        }
        TBCCR1 += 61440;
        break;
      case 4:
        WatchDog_Clear(); 
        TBCCR2 += 1024;
        Clear_ExternWatchdog();
        break;
      case 10:
        break;
      default:
        break;
    } 
}

