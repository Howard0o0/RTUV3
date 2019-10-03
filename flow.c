#include "msp430common.h"
#include "uart0.h"
#include "uart1.h"
#include "common.h"
#include "rom.h"
#include "flash.h"
#include "rtc.h"
#include "store.h"
#include "Sampler.h"
#include "flow.h"
#include "debug.h"
#include "hydrology.h"
#include "GPRS.h"

int FlowCheckSampleData(char* pstart,char* pend)
{
    //������ݴ洢�ϱ�� �±� 
    char _startIdx=*pstart;
    char _endIdx=*pend;
    
    if(RTC_ReadStartIdx(&_startIdx)<0 || RTC_ReadEndIdx(&_endIdx)<0 )
    {//������������������ʧ��
        TraceMsg("read idx error ,reGenerate .",1);
        
        if(RTC_RetrieveIndex()<0)
        {
            TraceMsg("reGen failed .",1);
            RTC_SetStartIdx(DATA_MIN_IDX);
            RTC_SetEndIdx(DATA_MIN_IDX);
        }
        TraceMsg("StartIdx:",0); 
        TraceInt4(_startIdx,1);
        TraceMsg("EndIdx:",0);
        TraceInt4(_endIdx,1);
        RTC_ReadStartIdx(&_startIdx);//���¶���
        RTC_ReadEndIdx(&_endIdx);//���¶���
    }
    //�±���ȷ��
    if( _endIdx<DATA_MIN_IDX || _endIdx >DATA_MAX_IDX ||
       _startIdx<DATA_MIN_IDX || _startIdx >DATA_MAX_IDX  )
    {
        TraceMsg("Idx bad .",1);
        return -1;
    }
    TraceMsg("startIdx=",1);
    TraceInt4(_startIdx,1);
    TraceMsg("endIdx=",1);
    TraceInt4(_endIdx,1);

    *pstart = _startIdx;
    *pend = _endIdx;
    
    return 0;
}

int FlowProcess()
{
 #if 1
    TraceMsg("FlowProcess",1);
    //������ݴ洢�ϱ�� �±� 
    char _startIdx=0;
    char _endIdx=0;
    char _send[120] = {0}; 
//    char _specSend[200] = {0};
    int  _ret=0;   
    int  _seek_num=0;//��ֹ��ѭ��
    
   _ret = FlowCheckSampleData(&_startIdx,&_endIdx);
    if (_ret !=0 )
    {
        return -1;   
    }
  
   while(1)
    {
    #if 1
        TraceMsg("read data in :",0);
        TraceInt4(_startIdx,1);
        if(_seek_num>DATA_ITEM_LEN)//Ѱ�ҵ����������Ѿ��������ֵ���˳�
        {
            TraceMsg("seek num out of range",1);
            break;
        }

        //����
        _ret = Store_ReadDataItem(_startIdx,_send,1);/////////////////////////////
        TraceMsg("read data is:",1);
        TraceMsg(_send, 1);
        if(_ret<0)
        {
            TraceMsg("can't read data ! very bad .",1);
            return -1; //�޷���ȡ���� ��ֱ������.
        }
        if(_ret==1)
        {//�����һ���Ѿ����͹�������, 
            TraceMsg("It's sended data",1);
            if(_startIdx == _endIdx)
            {//����Ƿ���  _endIdx, ����ǾͲ�����ѭ����. 
                TraceMsg("send data end",1);
                break;
            } 
            //������һ��λ��
            if(_startIdx >= DATA_MAX_IDX)  _startIdx=DATA_MIN_IDX;
            else   ++ _startIdx;//��һ����
            
            ++_seek_num;
            continue;
        }
        //�������ķ�������,������_idx
        //������һ�ε� _startIdx    
        //�����ø������ѷ���
        Store_MarkDataItemSended(_startIdx);
        
        if(_startIdx>=DATA_MAX_IDX) _startIdx=DATA_MIN_IDX;
        else   ++ _startIdx;//��һ����
        
        ++_seek_num;

        TraceMsg(_send, 1);
//        hydrologHEXProcess(_send,_ret,Test,_specSend);
//        hydrologHEXProcess(_send,_ret,EvenPeriodInformation,_specSend);
//        hydrologHEXProcess(_send,_ret,TimerReport,_specSend);
//        hydrologHEXProcess(_send,_ret,AddReport,_specSend);
//        hydrologHEXProcess(_send,_ret,Hour,_specSend);
//        hydrologHEXProcess(_send,_ret,ArtificialNumber,_specSend);
//        hydrologHEXProcess(_send,_ret,Realtime,_specSend);
//        hydrologHEXProcess(_send,_ret,Period,_specSend);
//        hydrologHEXProcess(_send,_ret,InquireArtificiaNumber,_specSend);
//        hydrologHEXProcess(_send,_ret,SpecifiedElement,_specSend);
//        hydrologHEXProcess(_send,_ret,ConfigurationModification,_specSend);
//        hydrologHEXProcess(_send,_ret,ConfigurationRead,_specSend);
//        hydrologHEXProcess(_send,_ret,ParameterModification,_specSend);
//        hydrologHEXProcess(_send,_ret,ParameterRead,_specSend);
//        hydrologHEXProcess(_send,_ret,WaterPumpMotor,_specSend);
//        hydrologHEXProcess(_send,_ret,SoftwareVersion,_specSend);
//        hydrologHEXProcess(_send,_ret,Status,_specSend);
//        hydrologHEXProcess(_send,_ret,InitializeSolidStorage,_specSend);
//        hydrologHEXProcess(_send,_ret,Reset,_specSend);
//        hydrologHEXProcess(_send,_ret,ChangePassword,_specSend);
//        hydrologHEXProcess(_send,_ret,SetClock,_specSend);
//        hydrologHEXProcess(_send,_ret,SetICCard,_specSend);
//        hydrologHEXProcess(_send,_ret,Pump,_specSend);
//        hydrologHEXProcess(_send,_ret,Valve,_specSend);
//        hydrologHEXProcess(_send,_ret,Gate,_specSend);
//        hydrologHEXProcess(_send,_ret,WaterSetting,_specSend);
//        hydrologHEXProcess(_send,_ret,Record,_specSend);
//        hydrologHEXProcess(_send,_ret,Time,_specSend);
//        DTU_SendData(_specSend,_ret);
      
        //�������,Ҫ����_startIdx. 
        RTC_SetStartIdx(_startIdx);
   #else
//        sprintf(_send,"%s","0909011230*A4096B4096");
//        hydrologHEXProcess(_send,Utility_Strlen(_send),TimerReport,_specSend);
//        hydrologHEXfree();
//
//        int _sendLen = 51;
//        char *_specSendChar = (char *)malloc(_sendLen*2+1);
//        if(_specSendChar == NULL)
//        {
//          TraceMsg("GPRS Send Malloc Failed", 1);
//        }
//        int _sendRet=0;
//        _sendRet = hex_2_ascii(_specSend, _specSendChar, _sendLen);
/////////////////////////////////////////////////////////////////////////
//      
//        if(GPRS_Write(_specSendChar,_sendRet) == GPRS_WRITE_FAILED)
//        {
//            UART1_Send("GPRS Send Failed, Wait 10S to Send Again",40,1);
//            free(_specSendChar);
//            _specSendChar = 0;
//            //retGPRS = GPRS_HardReboot();
//            System_Delayms(10000);
//            //continue;
//        }
//        
//        free(_specSendChar);
//        _specSendChar = 0;
//
//        System_Delayms(3000);
//
//        if(GPRS_CloseTCPLink() == TCP_Link_CLOSE_FAILED)
//        {
//            UART1_Send("Close TCP Connection Failed",27,1);
//            //continue;
//        }            
//        System_Delayms(3000);
/////////////////////////////////////////////////////////////////////////
//        hydrologHEXfree();
//
//        break;
//
   #endif
    }
    
    TraceMsg("Report done",1);
   
    return 0;
 #else
//    unsigned char testinputdata[] = {"0909011230*A4096B4096"};
//    unsigned char testoutputdata[200]={0};
//    int len = 0;
//
//    len = hydrologHEXProcess(testinputdata,sizeof(testinputdata),TimerReport,testoutputdata);
//
//   // QY_printf("data size is %d",len);
//    
//    hydrologHEXfree();
//    return 0;
 #endif
}


