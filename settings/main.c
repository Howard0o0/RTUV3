#include "msp430common.h"
#include "main.h"
#include "common.h"
#include "timer.h" 
#include "store.h"
#include "rtc.h"
#include "led.h" 
#include "console.h"
#include "Sampler.h"
#include "DTU.h"
#include "GSM.h"
#include "uart0.h"
#include "uart3.h"
#include "wifi_config.h"
//#include "flow.h"
//#include "hydrology.h"
#include <string.h>
#include "reportbinding.h"
#include "GTM900C.h"
#include "uart_config.h"
#include "hydrologytask.h"
#include "hydrologycommand.h"


int IsDebug = 0;

int main(void)
{
    
    char *data="hello";
    Restart_Init();  

    Select_Debug_Mode(0);
    
    TraceOpen();
    
    //秒、分、时、日、月、星期、年、控制字
    const char ch_time[8] = {0,34,16,27,8,2,19,0};
//    _RTC_SetTime(0,38,16,27,8,2,19,0);
//    _RTC_Write_OneByte(RegAddr_Control,0);
//    _RTC_Write_OneByte(RegAddr_CtlStat,0);
    
    /* RTC RAM test */
    const char write_ram_data = 0x12;    
    _RTC_WriteRAM(30,write_ram_data);
    
    while(1){

    //TraceMsg("Device Open !",1);
    
    //UART3_Send(data ,5,0);
    //printf("hahah\r\n");
        char time[8];
        _RTC_ReadTime(&time[0],&time[1],&time[2],&time[3],&time[4],&time[5],&time[6],&time[7]);
        printf("TIME:  %d:%d:%d %d-%d-%d  \r\n",time[2],time[1],time[0],time[6],time[4],time[3]);
        char control_reg;
        control_reg = _RTC_Read_OneByte(RegAddr_Control);
        printf("control:%x \r\n",control_reg);
        control_reg = _RTC_Read_OneByte(RegAddr_CtlStat);
        printf("control state:%x \r\n",control_reg);
        
        // int i;
        // char rtc_ram_data;
        // for(i=0;i<=30;i++)
        // {
        //     _RTC_WriteRAM(i,0xcd);
        //     rtc_ram_data = _RTC_ReadRAM(i);
        //     printf("[%d] ram: %x \r\n",i,rtc_ram_data);
        // }

        System_Delayms(1000);
    }
   /* 
    Main_Init();
    
    Sampler_Open();
    
    Hydrology_InitWaitConfig();
    
    while(1)
    {
        HydrologyTask();
   }*/
}

void Restart_Init()
{

    Clock_Init();               // CPU时钟初始化
        
    _EINT();                    //打开中断
    //IE1|=OFIE+NMIIE+ACCVIE;     //振荡器故障中断允许、NMI中断允许、FLASH存储器非法访问中断允许 
    SFRIE1|=OFIE+NMIIE+ACCVIE;
    WatchDog_Init();
    Led_Init();                 // 指示灯 初始化
//    Led1_WARN(); 
    
//    P9DIR |= BIT7;              //拉高232芯片控制引脚
//    P9OUT |= BIT7;
    P10DIR |= BIT0;              //拉高232芯片控制引脚
    P10OUT |= BIT0;
    TimerA_Init(30720);         // 系统定时器初始化

    TimerB_Init(61440); 

    Store_Init();               // 初始化ROM
    RTC_Open();
//    Sampler_Init();             //端口初始化,需要先初始化Store

    P3DIR &= ~BIT2;
    if(P3IN & BIT2)
    {
        IsDebug = 1;
    }
    else
    {
        IsDebug = 0;
    }
    Console_Open();
    
    return;
}

int Restart_GSMInit()
{
    GSM_Open();
    if(GSM_CheckOK()<0)
    { 
        System_Delayms(1000);
        GSM_Open();
        if(GSM_CheckOK()<0)
        { 
            System_Delayms(1000);
            GSM_Open();
            if(GSM_CheckOK()<0)
            {//无法工作就放弃了.
                GSM_Close(1);
                return -1;
            }
        }
    }
    
    char _phone[12];
    char _data[30];//30足够了
    
    _data[0]='$';
    if(Store_ReadDeviceNO(&_data[1])<0)
    {
    _data[1]='0';_data[2]='0';_data[3]='0';_data[4]='0';_data[5]='0';_data[6]='0';
    _data[7]='0';_data[8]='0';_data[9]='0';_data[10]='0';_data[11]='0';
    }
    
    Utility_Strncpy(&_data[12],"<restart#",9);
    if(Store_GSM_ReadCenterPhone(5,_phone)==0)
    {
        if(_phone[0]!='0'&&_phone[1]!='0'&&_phone[2]!='0')
        {
            GSM_SendMsgTxt(_phone,_data,21);
        }
    }
    GSM_Process(1,0);        // GSM 主流程 
    GSM_Close(0);//关闭 GSM
    System_Delayms(2000);//避免关机后过快开机导致失败.
    return 0;

}

int Restart_DTUInit()
{   
    char _data[UART3_MAXBUFFLEN];
    int _dataLen=0;
    int _repeats=0;
    int _out=0;
    //
    //这里增加一个DTU等待串口配置的过程.
    //
    if(trace_open==0) 
    {
        //如果没打开调试的话,串口是要额外打开的
        //Console_Open(); 
    }
    UART3_ClearBuffer();
    //Console_WriteStringln("ACK");

    if(UART3_RecvLineLongWait(_data,UART3_MAXBUFFLEN,&_dataLen)==0)
    {
        //如果等到串口数据, 就进入配置状态.等待15秒钟
        //Console_WriteStringln("waiting for 15 seconds .");
        if(Main_ProcCommand(_data,_dataLen,NULL)==3)
        {//返回了一个SYN,就进入配置状态
            while(1)
            {
                _repeats=0;
                while(UART3_RecvLineLongWait(_data,UART3_MAXBUFFLEN,&_dataLen)<0)
                {
                    ++_repeats;
                    if(_repeats>4)
                    {
                        _out=1;
                        break;
                    }
                }
                if(_out!=0)
                    break; 
                if(Main_ProcCommand(_data,_dataLen,NULL)==2)
                {//表示退出配置状态
                    break;
                }
            }
        }
    }
    //当不在调试状态的时候要关闭的
    if(trace_open==0)
    {//如果调试是关闭的,串口到此要关闭
    //Console_Close(); 
    } 
    
    return 0;
 
}

int WorkMode_Init(char* ptype)
{
    char _curType , _selType ;
    int _ret;

#if 0
    //获取选择工作状态    DTU/GSM   232 
    P3DIR &= ~BIT3;   //P33为输入
    P3DIR &= ~BIT2;   //P32为输入
    if(P3IN & BIT2)
    {//P32为高
        _selType='D';
    }
    else
    {
        _selType='G';
    } 
#endif
    _selType = 'S' ;//GPRS模式 GTM900
    switch(_selType)
    {
      case 'G':
        g_main_type=MAIN_TYPE_GSM;
        TraceMsg("Device is GSM Mode !",1);
        break;
      case 'D':
        g_main_type=MAIN_TYPE_DTU;
        TraceMsg("Device is DTU Mode !",1);
        break;

      case 'S':
        g_main_type=MAIN_TYPE_GTM900;
        TraceMsg("Device is GPRS Mode !",1);
        break;
        
      default:
        //这里出现问题.就重启 
        
        TraceMsg("Bad Mode !",1);
        System_Reset();
        break;
    }  

     //判断当前工作状态,   DTU/GSM      
    if(Store_ReadSystemType(&_curType)<0)
    {//如果无法读出 保存的模式,就不判断是否 重新设置了.
        _ret =0;
    }
    else
    {
        _ret  = _selType != _curType ? 1 : 0 ;       
    }  

    *ptype = _selType;
    
    return _ret;
    
}

void Main_Init()
{ //时钟使用8M 
    //开中断,以防之前部分有错误导致中断被关闭
    EnInt();
    TraceOpen();
    RTC_Open();       // 打开RTC
    TimerB_Clear();
    WatchDog_Clear();   // 清除复位计数器 
    //在里再次运行这个函数  增加可靠性.
    Sampler_Init();     
}

void Main_GotoSleep()
{
    //开中断,以防之前部分有错误导致中断被关闭
    EnInt();
    
    TraceMsg("Device is going to sleep !",1);
    //关闭外围,进入休眠
    RTC_Close();
    Console_Close();
//    DTU_Close();
//    GSM_Close(1); 
//    call gprs close
    Sampler_Close(); 
    TimerB_Clear();
    WatchDog_Clear();
    //TraceMsg("Device sleep !",1); //此处串口已经关闭
    LPM2;
//    LPM3;
}


