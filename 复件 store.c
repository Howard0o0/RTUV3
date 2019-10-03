//////////////////////////////////////////////////////
//     �ļ���: store.c
//   �ļ��汾: 1.0.0
//   ����ʱ��: 09��11��30��
//   ��������:  
//       ����: ����
//       ��ע: 
//
//       1. ������֯��ʽ.
//       ������:
//           У���ֽ�(1) + ʱ�䴮(10) + ģ��ֵ��(8*2) + ����ֵ��(4*3) + ������(1) 
//           �������ݲ���Ҫ������
//           ����ֵ  ���� ���� 3�ֽ�.
//       ʱ�䴮:
//           ������������ʱʱ�ַ�
//       ģ��ֵ��: (������ A ~ H)
//           ������(1) + ��ֵ(2�ֽ�int����)
//       ����ֵ��: (������ I ~ L)
//           ������(1) + ��ֵ(2�ֽ�int����)
//        
//       2. �ϱ���ʽ
//       $00000000000>0909091230*A1234B1234C1234D1234E1234F1234G1234H1234I000000J000000K000000L000000
//       M1N1O1P1Q1R1S1T1#
//////////////////////////////////////////////////////
 
#include <msp430x16x.h>

#include "rom.h"
#include "flash.h"
#include "store.h"
#include "common.h" 
#include "rtc.h" 


////////////////////////////////////////
//  ROM�������  ��ַΪint����,11λ 
//  ��ʼ��ַ: 0x0000  ��ߵ�ַ: 0x07FF 
////////////////////////////////////////


//    0x0000 ~ 0x0035   �豸��������
//    0x0036 ~ 0x0079   ��ϵͳ����
//    0x00A0 ~ 0x07FF   ������ 

#define           BASE_ADDR    0x0000
 

//�����������ֽ�               1�ֽ�,  0x0000  
#define         CONFIG_ADDR    BASE_ADDR 

//�豸��                       11�ֽ�, 0x0001 ~ 0x000B
#define       DEVICENO_ADDR    (BASE_ADDR + 1 )
#define       DEVICENO_LEN     11
//����                         4�ֽ�,  0x000C ~ 0x000F   
#define       PASSWORD_ADDR    (BASE_ADDR + 12)
#define       PASSWORD_LEN     4
//���� ʱ����ģʽ            2�ֽ�,  0x0010 ~ 0x0011
#define REPORTTIME_MODE_ADDR   (BASE_ADDR + 16)
#define REPORTTIME_MODE_LEN    2 
//���� ʱ����ģʽ            2�ֽ�,  0x0012 ~ 0x0013
#define   SAVETIME_MODE_ADDR   (BASE_ADDR + 18)
#define   SAVETIME_MODE_LEN    2 


//����������  ���ݵĻ���ַ  
#define DATA_LIMIT_BASE_ADDR   (BASE_ADDR + 20)
#define DATA_MAX_BASE_ADDR     DATA_LIMIT_BASE_ADDR
//                                    0x0015
#define DATA_MIN_BASE_ADDR     (DATA_LIMIT_BASE_ADDR + 2 )

#define     DATA_LIMIT_LEN     2
//����1��������                2�ֽ�,  0x0014~0x0015  
#define     DATA1_MAX_ADDR     DATA_MAX_BASE_ADDR
//����1��������                2�ֽ�,  0x0016~0x0017  
#define     DATA1_MIN_ADDR     DATA_MIN_BASE_ADDR
//����2��������                2�ֽ�,  0x0018~0x0019  
#define     DATA2_MAX_ADDR     (DATA_MAX_BASE_ADDR+4)
//����2��������                2�ֽ�,  0x001A~0x001B  
#define     DATA2_MIN_ADDR     (DATA_MIN_BASE_ADDR+4)
//����3��������                2�ֽ�,  0x001C~0x001D  
#define     DATA3_MAX_ADDR     (DATA_MAX_BASE_ADDR+8)
//����3��������                2�ֽ�,  0x001E~0x001F  
#define     DATA3_MIN_ADDR     (DATA_MIN_BASE_ADDR+8)
//����4��������                2�ֽ�,  0x0020~0x0021  
#define     DATA4_MAX_ADDR     (DATA_MAX_BASE_ADDR+12)
//����4��������                2�ֽ�,  0x0022~0x0023  
#define     DATA4_MIN_ADDR     (DATA_MIN_BASE_ADDR+12)
//����5��������                2�ֽ�,  0x0024~0x0025  
#define     DATA5_MAX_ADDR     (DATA_MAX_BASE_ADDR+16)
//����5��������                2�ֽ�,  0x0026~0x0027  
#define     DATA5_MIN_ADDR     (DATA_MIN_BASE_ADDR+16)
//����6��������                2�ֽ�,  0x0028~0x0029  
#define     DATA6_MAX_ADDR     (DATA_MAX_BASE_ADDR+20)
//����6��������                2�ֽ�,  0x002A~0x002B  
#define     DATA6_MIN_ADDR     (DATA_MIN_BASE_ADDR+20)
//����7��������                2�ֽ�,  0x002C~0x002D  
#define     DATA7_MAX_ADDR     (DATA_MAX_BASE_ADDR+24)
//����7��������                2�ֽ�,  0x002E~0x002F  
#define     DATA7_MIN_ADDR     (DATA_MIN_BASE_ADDR+24)
//����8��������                2�ֽ�,  0x0030~0x0031  
#define     DATA8_MAX_ADDR     (DATA_MAX_BASE_ADDR+28)
//����8��������                2�ֽ�,  0x0032~0x0033  
#define     DATA8_MIN_ADDR     (DATA_MIN_BASE_ADDR+28)

//ͨ������                     3�ֽ�
#define CHANNEL_CONFIG_ADDR    (DATA_LIMIT_BASE_ADDR + 32 )
//                             0x0034
#define     ANALOG_SEL_ADDR    CHANNEL_CONFIG_ADDR
//                             0x0035  ��4λ����
#define     PULSE_SEL_ADDR     (CHANNEL_CONFIG_ADDR + 1)
//                             0x0036
#define     ONOFF_SEL_ADDR     (CHANNEL_CONFIG_ADDR + 2)
//                             0x0037
#define     WORK_MODE_ADDR     (CHANNEL_CONFIG_ADDR + 3)
//                             0x0038
#define     SYSTEM_TYPE_ADDR   (CHANNEL_CONFIG_ADDR + 4) 
//                             0x0039
#define PULSE_RATE_BASE_ADDR   (CHANNEL_CONFIG_ADDR + 5)
#define     PULSE1_RATE_ADDR   PULSE_RATE_BASE_ADDR
//                             0x003A 
#define     PULSE2_RATE_ADDR   (CHANNEL_CONFIG_ADDR + 6)
//                             0x003B
#define     PULSE3_RATE_ADDR   (CHANNEL_CONFIG_ADDR + 7)
//                             0x003C
#define     PULSE4_RATE_ADDR   (CHANNEL_CONFIG_ADDR + 8)

 
//  �Լ���9����ʾ ����������

//                             0x003D 
#define PULSE_RANGE_BASE_ADDR  (CHANNEL_CONFIG_ADDR + 9)
#define   PULSE1_RANGE_ADDR    PULSE_RANGE_BASE_ADDR    
//                             0x003E
#define   PULSE2_RANGE_ADDR    (PULSE_RANGE_BASE_ADDR + 1)
//                             0x003F   
#define   PULSE3_RANGE_ADDR    (PULSE_RANGE_BASE_ADDR + 2)
//                             0x0040   
#define   PULSE4_RANGE_ADDR    (PULSE_RANGE_BASE_ADDR + 3)

//                             0x0041
#define      IO_DIR_CFG_ADDR   (CHANNEL_CONFIG_ADDR + 13)
//                             0x0042
#define    IO_LEVEL_CFG_ADDR   (CHANNEL_CONFIG_ADDR + 14)

#define     Config_END_ADDR    0x004F


//////////////////////////////////////
//
//  GSM ϵͳ�ķ���
//
///////////////////////////////////////
//                                     0x0050
#define     GSM_START_ADDR             0x0050


#define     GSM_CENTERPHONE_BASE_ADDR  GSM_START_ADDR
#define     GSM_CENTER_PHONE_LEN       11

//                                     0x0050    
//   ��������1                         11�ֽ�,   0x0050 -5A
#define     GSM_CENTER1_PHONE_ADDR     ( GSM_CENTERPHONE_BASE_ADDR )
//   ��������2                         11�ֽ�,   0x005B 
#define     GSM_CENTER2_PHONE_ADDR     ( GSM_CENTERPHONE_BASE_ADDR + 11)
//   ��������3                         11�ֽ�,   0x0066
#define     GSM_CENTER3_PHONE_ADDR     ( GSM_CENTERPHONE_BASE_ADDR + 22)
//   ��������4                         11�ֽ�,   0x0071
#define     GSM_CENTER4_PHONE_ADDR     ( GSM_CENTERPHONE_BASE_ADDR + 33)
//   ��������                          11�ֽ�,   0x007C
#define      GSM_CONFIG_PHONE_ADDR     ( GSM_CENTERPHONE_BASE_ADDR + 44)
//   �����˵绰                        11�ֽ�,   0x0087
#define      GSM_LEADER_PHONE_ADDR     ( GSM_CENTERPHONE_BASE_ADDR + 55) 
//                                     0x0091  
#define     GSM_CENTERPHONE_END_ADDR   ( GSM_CENTERPHONE_BASE_ADDR + 65 )



///////////////////////////////////////
//
//  DTU ϵͳ�ķ��� 
//
///////////////////////////////////////

//                                     0x0050 
#define     DTU_START_ADDR             0x0050
#define     DTU_CENTERIP_BASE_ADDR     DTU_START_ADDR
#define     CENTER_IP_LEN              6      
//   ��������1                         6�ֽ�,  0x0050 ~  
#define     CENTER1_IP_ADDR           (DTU_CENTERIP_BASE_ADDR)
//   ��������2                         6�ֽ�,  0x0056 ~  
#define     CENTER2_IP_ADDR            (DTU_CENTERIP_BASE_ADDR + 6)
//   ��������3                         6�ֽ�,  0x005B ~  
#define     CENTER3_IP_ADDR            (DTU_CENTERIP_BASE_ADDR + 12)
//   ��������4                         6�ֽ�,  0x0050 ~  
#define     CENTER4_IP_ADDR            (DTU_CENTERIP_BASE_ADDR + 18)
//   ��������5                         6�ֽ�,  0x0055 ~  
#define     CENTER5_IP_ADDR            (DTU_CENTERIP_BASE_ADDR + 24)


//////////////////////////////////////
//
//  485 ϵͳ�ķ��� 
//
///////////////////////////////////////


//������
//                                    0x00A0
#define     DATA_START_ADDR           0x00A0
#define     DATA_ITEM_LEN             40
#define     DATA_ITEM_MAX             DATA_MAX_IDX
//#define     DATA_END_ADDR        
//( DATA_START_ADDR + DATA_ITEM_MAX * DATA_ITEM_LEN)

//��ʼ��
void Store_Init()
{
    ROM_Init();
} 
//  ���� 4096�������ִ�
int Store_ReadDataMaxStr(int _index ,char * _dest)
{
    if( _index < 1 || _index >8)
        return -2;
    int _addr = DATA_MAX_BASE_ADDR + ( _index - 1 ) * DATA_LIMIT_LEN * 2;
    int _repeats=0;char _temp[2]; 
    while(ROM_ReadBytes( _addr , _temp,DATA_LIMIT_LEN )!=0)
    {
        if(_repeats>2)
        {
            return -1;
        }
        ++_repeats;
    }
    unsigned int _value=(((unsigned int) _temp[0])<<8); _value += _temp[1];
    Utility_UintToStr4(_value,_dest);
    return 0;
}

int Store_ReadDataMaxInt(int _index, int * _pDestInt)
{
    if( _index < 1 || _index >8)
        return -2;
    int _addr = DATA_MAX_BASE_ADDR + ( _index - 1 ) * DATA_LIMIT_LEN * 2;
    int _repeats=0; char _temp[2];
    while(ROM_ReadBytes(_addr,_temp, DATA_LIMIT_LEN )!=0)
    {
        if( _repeats > 2 )
        { 
            return -1;
        }
        ++ _repeats;
    }//ת��Ϊint
    (*_pDestInt)= (((unsigned int)_temp[0])<<8 ) ;(*_pDestInt)+=_temp[1];//�ߵ�λ�ֽ� 
    return 0;
}


int Store_SetDataMaxInt(int _index, const int _max)
{
    if( _max > 4096 || _max < 0 )
        return -2;
    if( _index < 1 || _index > 8 )
        return -2;
    int _addr = DATA_MAX_BASE_ADDR + ( _index - 1 ) * DATA_LIMIT_LEN * 2 ;
    int _repeats=0; char _temp[2];
    _temp[0]= (_max>>8); _temp[1]=_max&0x00FF; 
    while(ROM_WriteBytes(_addr,_temp, DATA_LIMIT_LEN )!=0)
    {
        if(_repeats>2)
        {
            return -1;
        }
        ++ _repeats;
    }
    return 0;
}

//4�ֽ��ַ�����
int Store_ReadDataMinStr(int _index, char * _dest)
{
    if( _index < 1  || _index > 8 )
        return -2;
    int _addr = DATA_MIN_BASE_ADDR + ( _index - 1 ) * DATA_LIMIT_LEN * 2;
    int _repeats=0;char _temp[2]; 
    while(ROM_ReadBytes(_addr, _temp,DATA_LIMIT_LEN )!=0)
    {
        if(_repeats>2)
        { 
            return -1;
        }
        ++_repeats;
    }
    unsigned int _value=(((unsigned int)_temp[0])<<8); _value += _temp[1];
    Utility_UintToStr4(_value,_dest);  
    return 0;
}


int Store_ReadDataMinInt(int _index, int * _pDestInt)
{
    if( _index < 1  || _index > 8 )
        return -2;
    int _addr = DATA_MIN_BASE_ADDR + ( _index - 1 ) * DATA_LIMIT_LEN * 2;
    int _repeats=0;char _temp[2];
    while(ROM_ReadBytes(_addr, _temp, DATA_LIMIT_LEN )!=0)
    {
        if(_repeats>2)
        {
            return -1;
        }
        ++ _repeats;
    }//ת��ΪInt
    (*_pDestInt)=((unsigned int)_temp[0])<<8;  (*_pDestInt) += _temp[1];//�ߵ�λ�ֽ� 
    return 0;
}

//���ø�ʽΪ4068
int Store_SetDataMinInt(int _index, const int _min)
{
    if( _index < 1  || _index > 8 )
        return -2;
    int _addr = DATA_MIN_BASE_ADDR + ( _index - 1 ) * DATA_LIMIT_LEN * 2;
    int _repeats=0;char _temp[2];
    _temp[0]=(_min>>8); _temp[1]=_min & 0x00FF ; //��λ�ֽ�  
    while(ROM_WriteBytes(_addr,_temp, DATA_LIMIT_LEN )!=0)
    {
        if(_repeats>2)
        {
            return -1;
        }
        ++_repeats;
    }
    return 0;
}


int Store_ReadConfig(char * _dest)//  1���ֽ�
{ 
    int _repeats=0;
    while(ROM_ReadByte(CONFIG_ADDR,_dest)!=0)
    {
        if(_repeats>2)
        { 
            return -1;
        }
        ++ _repeats;
    } 
    return 0;
}


int Store_SetConfig(char _config)
{
     int _repeats=0;
     while(ROM_WriteByte(CONFIG_ADDR,_config)!=0)
     {
         if(_repeats>2)
         { 
             return -1;
         }
         ++_repeats;
     } 
     return 0;
} 
// 11���ַ����ֽ�
int Store_ReadDeviceNO(char *_dest)
{
  int _repeats=0;
  while(ROM_ReadBytes(DEVICENO_ADDR,_dest,DEVICENO_LEN )!=0)
  {
    if(_repeats>2)
    {  
      return -1;
    }
    ++_repeats;
  }
  return 0;
}
// 3���ַ����ֽ�
int Store_SetDeviceNO(const char * _src)
{
  int _repeats=0;
  while(ROM_WriteBytes(DEVICENO_ADDR, _src,DEVICENO_LEN )!=0)
  {
    if(_repeats>2)
    { 
      return -1;
    }
    ++_repeats;
  }  
  return 0;  
}
// 4���ַ����ֽ�
int Store_ReadPassword(char *_dest)
{ 
  int _repeats=0;
  while(ROM_ReadBytes(PASSWORD_ADDR,_dest,PASSWORD_LEN )!=0)
  {
    if(_repeats>2)
    {  
      return -1;
    }
    ++_repeats;
  }
  return 0;  
} 


// 4���ַ����ֽ�
int Store_SetPassword(const char * _src)
{
    int _repeats=0;
    while(ROM_WriteBytes(PASSWORD_ADDR,_src, PASSWORD_LEN )!=0)
    {
        if(_repeats>2)
        { 
            return -1;
        }
        ++_repeats;
    } 
    return 0;  
} 


//1���ַ����ֽ�
int  Store_ReadWorkMode(char * _dest)
{
    int _repeats=0;
    while(ROM_ReadByte(WORK_MODE_ADDR , _dest)<0)
    {
        if(_repeats>2)
            return -1;
        ++ _repeats;

    }
    return 0;  
}

// '1'  '0'
int  Store_SetWorkMode(char _src)
{
    int _repeats=0; 
    while(ROM_WriteByte(WORK_MODE_ADDR , _src)<0)
    {
        
        if( _repeats >2)
            return -1;
        ++ _repeats;
         
    }
    return 0; 
}
int  Store_ReadSystemType(char * _dest)
{
    int _repeats=0;
    while(ROM_ReadByte(SYSTEM_TYPE_ADDR, _dest)<0)
    {
        if( _repeats >2)
            return -1;
        ++ _repeats;
    }
    return 0;
}
int  Store_SetSystemType(char _src)
{
    int  _repeats=0;
    while(ROM_WriteByte(SYSTEM_TYPE_ADDR, _src)<0)
    {
        if( _repeats<2)
            return -1;
        ++ _repeats;
    }
    return 0;
}

int  Store_ReadAnalogSelect(char * _dest)
{
    int _repeats=0;
    while(ROM_ReadByte(ANALOG_SEL_ADDR, _dest)<0)
    {
        if( _repeats >2)
            return -1;
        ++ _repeats;
    }
    return 0;
}
int  Store_SetAnalogSelectStr(const char * _src)
{
    char _temp=0x00;
    //��_src�γ�һ��charд��
    for(int i=7;i>=0;--i)
    {
        //��һλ;
        _temp<<=1;
        if(_src[i]=='0')
            _temp &= 0xFE;//�����һλ��0;
        else
            _temp |= 0x01;//�����һλ��1;
        
    } 
    if(Store_SetAnalogSelect(_temp)<0)
        return -1;
    return 0;
}
int  Store_SetAnalogSelect(char _src)
{
    int  _repeats=0;
    while(ROM_WriteByte(ANALOG_SEL_ADDR, _src)<0)
    {
        if( _repeats<2)
            return -1;
        ++ _repeats;
    }
    return 0;
}

int  Store_ReadPulseSelect(char * _dest)
{
    int _repeats=0;
    while(ROM_ReadByte(PULSE_SEL_ADDR, _dest)<0)
    { 
        if( _repeats >2)
            return -1;
        ++ _repeats;
    }
    return 0;
}

int  Store_SetPulseSelectStr(const char * _src)
{
    char _temp=0x00;
    //��_src�γ�һ��charд��
    for(int i=0;i<4;++i)
    {
        
        
        if(_src[i]=='0')
            _temp &= 0xF7;//��������4λ��0;
        else
            _temp |= 0x08;//��������4λ��1;
        
        //��һλ;
        _temp<<=1;
    }
    //ǰ4λ��Ӧ��Ϊѡ��
    if(Store_SetPulseSelect(_temp)<0)
        return -1;
    return 0;
}

int  Store_SetPulseSelect(char _src)
{
    int  _repeats=0;
    while(ROM_WriteByte(PULSE_SEL_ADDR, _src)<0)
    {
        if( _repeats<2)
            return -1;
        ++ _repeats;
    }
    return 0;
}

int  Store_ReadIoSelect(char * _dest)
{
    int _repeats=0;
    while(ROM_ReadByte(ONOFF_SEL_ADDR, _dest)<0)
    {
        if( _repeats >2)
            return -1;
        ++ _repeats;
    }
    return 0;
}
int  Store_SetIoSelect(char _src)
{
    int  _repeats=0;
    while(ROM_WriteByte(ONOFF_SEL_ADDR, _src)<0)
    {
        if( _repeats<2)
            return -1;
        ++ _repeats;
    }
    return 0;
}
int  Store_SetIoSelectStr(const char * _src)
{
    char _temp=0x00;
    //��_src�γ�һ��charд��
    for(int i=7;i>=0;--i)
    {
        //��һλ;
        _temp<<=1;
        
        if(_src[i]=='0')
            _temp &= 0xFE;//�����һλ��0;
        else
            _temp |= 0x01;//�����һλ��1;
        
    } 
    if(Store_SetIoSelect(_temp)<0)
        return -1;
    return 0;
}



int  Store_ReadPulseRate(int _index, char * _dest)
{
    if( _index < 1 || _index>4)
        return -2;
    int _addr = PULSE_RATE_BASE_ADDR + (_index-1);
    int _repeats=0;
    while(ROM_ReadByte( _addr , _dest)<0)
    {
        if( _repeats >2)
            return -1;
        ++ _repeats;
    }
    return 0; 
}

int  Store_SetPulseRate(int _index, char _src)
{
    if( _index < 1 || _index>4)
        return -2;
    int _addr = PULSE_RATE_BASE_ADDR + (_index-1); 
    int  _repeats=0;
    while(ROM_WriteByte( _addr , _src)<0)
    {
        if( _repeats<2)
            return -1;
        ++ _repeats;
    }
    return 0;
}


int  Store_ReadPulseRange(int _index, char *_dest)
{//����9�ĸ���  2����
    if( _index < 1 || _index>4)
        return -2;
    int _addr = PULSE_RANGE_BASE_ADDR + (_index-1);
    int _repeats=0;
    while(ROM_ReadByte(_addr,_dest)<0)
    {
        if(_repeats>2)
            return -1;
        ++ _repeats;
    }
    return 0;
}
int  Store_SetPulseRange(int _index, char  _src)
{//д�� 9�ĸ��� 2����
    if( _index < 1 || _index>4)
        return -2;
    int _addr = PULSE_RANGE_BASE_ADDR + (_index-1); 
    int  _repeats=0;
    while(ROM_WriteByte( _addr , _src)<0)
    {
        if( _repeats<2)
            return -1;
        ++ _repeats;
    }
    return 0;
}
int  Store_ReadPulseRangeBytes(int _index, char *_dest)
{//���� ��Ӧ�� 3���ֽ�
    if( _index < 1|| _index>4)
        return -2;
    char _temp=0x00;
    int _addr = PULSE_RANGE_BASE_ADDR + (_index-1);
    int _repeats=0;
    while(ROM_ReadByte( _addr , &_temp)<0)
    {
        if( _repeats >2)
            return -1;
        ++ _repeats;
    }
    switch(_temp)
    {
      case 0://0��9,���ڴ������δ���õ�ʱ��.
        _dest[0]=0x00;_dest[1]=0x00;_dest[2]=0x00;
        break;
      case 1:
        _dest[0]=0x00;_dest[1]=0x00;_dest[2]=0x09;
        break;
      case 2:
        _dest[0]=0x00;_dest[1]=0x00;_dest[2]=0x63;
        break;
      case 3:
        _dest[0]=0x00;_dest[1]=0x03;_dest[2]=0xE7;
        break;
      case 4:
        _dest[0]=0x00;_dest[1]=0x27;_dest[2]=0x0F;
        break;
      case 5:
        _dest[0]=0x01;_dest[1]=0x86;_dest[2]=0x9F;
        break;
      case 6:
        _dest[0]=0x0F;_dest[1]=0x42;_dest[2]=0x3F;
        break;
      case 7:
        _dest[0]=0x98;_dest[1]=0x96;_dest[2]=0x7F;
        break;
      default:
        //�����9�ĸ���,��������Ϊ0
        Store_SetPulseRange(_index,0);
        _dest[0]=0x00;_dest[1]=0x00;_dest[2]=0x00;
        break;
    }
    return 0;
}

//8���ַ�����
int Store_ReadIoDirConfigStr(char * _dest)
{
    char _temp;
    if(Store_ReadIoDirConfig(&_temp)<0)
        return -1;
    for(int i=0;i<8;++i)
    {
        if(_temp & 0x01)
            _dest[i]='1';
        else
            _dest[i]='0';
        _temp >>= 1;
    }
    return 0;
}
int  Store_ReadIoDirConfig(char * _dest)
{
    int _repeats=0;
    while(ROM_ReadByte(IO_DIR_CFG_ADDR, _dest)<0)
    {
        if( _repeats >2)
            return -1;
        ++ _repeats;
    }
    return 0;
}
int  Store_SetIoDirConfig(char _src)
{
    int  _repeats=0;
    while(ROM_WriteByte(IO_DIR_CFG_ADDR, _src)<0)
    {
        if( _repeats<2)
            return -1;
        ++ _repeats;
    }
    return 0;
}
int  Store_SetIoDirConfigStr(char * _src)
{
    char _temp=0x00;
    for(int i=7;i>=0;--i)
    {
        if(_src[i]=='0')
            _temp &= 0xFE;//�����һλΪ0
        else
            _temp |= 0x01;//�����һλΪ1
        
        _temp <<=1;
    }
    if(Store_SetIoDirConfig(_temp)<0)
        return -1;
    return 0;
}
//8���ַ�����
int  Store_ReadIoLevelConfigStr(char * _dest)
{
    char _temp;
    if(Store_ReadIoLevelConfig(&_temp)<0)
        return -1;
    for(int i=0;i<8;++i)
    {
        if(_temp & 0x01)
            _dest[i]='1';
        else
            _dest[i]='0';
        _temp >>= 1;
    }
    return 0;
}

int  Store_ReadIoLevelConfig(char * _dest)
{
    int _repeats=0;
    while(ROM_ReadByte(IO_LEVEL_CFG_ADDR, _dest)<0)
    {
        if( _repeats >2)
            return -1;
        ++ _repeats;
    }
    return 0;
}
int  Store_SetIoLevelConfig(char _src)
{
    int  _repeats=0;
    while(ROM_WriteByte(IO_LEVEL_CFG_ADDR, _src)<0)
    {
        if( _repeats<2)
            return -1;
        ++ _repeats;
    }
    return 0;
}
int  Store_SetIoLevelConfigStr(char * _src)
{    
    char _temp=0x00;
    for(int i=7;i>=0;--i)
    {
        if(_src[i]=='0')
            _temp &= 0xFE;//�����һλΪ0
        else
            _temp |= 0x01;//�����һλΪ1
        
        _temp <<=1;
    }
    if(Store_SetIoLevelConfig(_temp)<0)
        return -1;
    return 0;
}

//2���ַ����ֽ�
int Store_ReadReportTimeMode(char * _dest)
{
  int _repeats=0;
  while(ROM_ReadBytes(REPORTTIME_MODE_ADDR,_dest,REPORTTIME_MODE_LEN)!=0)
  {
    if(_repeats>2)
    { 
      return -1;
    }
    ++_repeats;
  } 
  return 0;  
}


//2���ַ����ֽ�
int Store_SetReportTimeMode(const char * _src)
{
  int _repeats=0;
  while(ROM_WriteBytes(REPORTTIME_MODE_ADDR,_src,REPORTTIME_MODE_LEN)!=0)
  {
    if(_repeats>2)
    {  
      return -1;
    }
    ++_repeats;
  } 
  return 0;  
}


//2���ַ����ֽ�
int Store_ReadSaveTimeMode(char *_dest)
{
  int _repeats=0;
  while(ROM_ReadBytes(SAVETIME_MODE_ADDR,_dest,SAVETIME_MODE_LEN)!=0)
  {
    if(_repeats>2)
    {
      return -1;
    }
    ++_repeats;
  }
  return 0;
}

//2���ַ����ֽ�
int Store_SetSaveTimeMode(const char * _src)
{
    int _repeats=0;
    while(ROM_WriteBytes(SAVETIME_MODE_ADDR , _src , SAVETIME_MODE_LEN )!=0)
    {
        if(_repeats>3)
        { 
            return -1;
        }
        ++_repeats;
    } 
    return 0;
}
//////////////////////////////////////////////////////////////
//     GSM ����
//////////////////////////////////////////////////////////////
int  Store_GSM_ReadCenterPhone(int _index, char *_dest)
{
    if(_index< 1||_index>6)
        return -2;
    int _repeats=0; 
    int _addr=GSM_CENTERPHONE_BASE_ADDR + (_index-1)*GSM_CENTER_PHONE_LEN;
    while(ROM_ReadBytes(_addr,_dest,GSM_CENTER_PHONE_LEN)!=0)
    {
        if(_repeats>2)
        { 
            return -1;
        }
        ++_repeats;
    }
    return 0;
}

int  Store_GSM_SetCenterPhone(int _index, char *_src)
{
    if(_index< 1 || _index>6)
        return -2;
    int _repeats=0;
    int _addr=GSM_CENTERPHONE_BASE_ADDR + (_index-1)*GSM_CENTER_PHONE_LEN;
    while(ROM_WriteBytes(_addr,_src,GSM_CENTER_PHONE_LEN)!=0)
    {
        if(_repeats>2)
        {
            return -1;
        }
        ++_repeats;
    }
    return 0;
}


//
//
//  ���ݱ����� ��������
//
//
//
int  Store_CheckDataItemSended(int _index) //��鷢�ͱ��
{
    if( _index < DATA_MIN_IDX || _index > DATA_MAX_IDX)
        return -2;
    char _temp;
    int _addr=DATA_START_ADDR + (_index -1)*DATA_ITEM_LEN; //
    if(ROM_ReadByte(_addr,&_temp)<0)
    {
        if(ROM_ReadByte(_addr,&_temp)<0)
            return -1;
    }
    if( _temp == 0x0F )
        return 1;
    else
        return 0; 
}
int  Store_ClearWork()
{
    //����������ȫ����ΪĬ��
    for(int i=1;i<=8;++i)
    {
        if(Store_SetDataMaxInt(i,4096)<0)
            return -1;
        if(Store_SetDataMinInt(i,0)<0)
            return -1;
    }
    //������ȫ����Ϊ�Ѿ�����
    for(int i=DATA_MIN_IDX;i<=DATA_MAX_IDX;++i)
    {
        if(Store_MarkDataItemSended(i)<0)
            return -1;
    }
    return 0;
}

int  Store_MarkDataItemSended(int _index)   //���÷��ͱ��
{
    if( _index < DATA_MIN_IDX || _index > DATA_MAX_IDX)
        return -2;
    int _addr=DATA_START_ADDR + (_index -1)*DATA_ITEM_LEN; //
    if(ROM_WriteByte(_addr,0x0F)<0) 
    {
         if(ROM_WriteByte(_addr,0x0F)<0)
             return -1;
    }
    return 0;
}
//  д���ݺ��� ֱ��д
int  Store_WriteDataItem(char _index, const char * _src)
{//DATA_ITEM_LEN=40
    if( _index < DATA_MIN_IDX || _index > DATA_MAX_IDX)
        return -2;
    //��һ��δ�����(��У���ֽ�)��_src�ṩ
    int _addr = DATA_START_ADDR + ( _index -1 ) * DATA_ITEM_LEN; 
    int _repeats=0;
    while(ROM_WriteBytes(_addr,_src,16)<0)
    {
        if(_repeats>2)
            return -1;
        ++ _repeats;
    }
    _repeats=0;
    _addr += 16;
    _src +=16;
    while(ROM_WriteBytes(_addr,_src,16)<0)
    {
        if(_repeats>2)
            return -1;
        ++ _repeats;
    }
    _repeats=0;
    _addr +=16;
    _src +=16;
    while(ROM_WriteBytes(_addr,_src,DATA_ITEM_LEN-32)<0)
    {
        if(_repeats>2)
            return -1;
        ++ _repeats;
    }
    return 0;
}
int  Store_WriteDataItemAuto(const char * _src)
{
    char _endIdx=0x00;
    if(RTC_ReadEndIdx(&_endIdx)<0)//
    {//��ȡ��д��Ǵ���
        if(RTC_RetrieveIndex()<0)
            return -1;//�������ɱ��ʧ��
        if(RTC_ReadEndIdx(&_endIdx)<0)
            return -1;//�Ծɴ���
    }
    if(Store_WriteDataItem(_endIdx,_src)<0)
        return -1;
    if( _endIdx == DATA_MAX_IDX)
        _endIdx = DATA_MIN_IDX;
    else
        ++_endIdx;
    RTC_SetEndIdx(_endIdx);//����_endIdx
    return 0;
}
/*
int  Store_ReadDataItemAuto(char * _dest)
{
    char _startIdx=0x00;
    if(RTC_ReadStartIdx(&_startIdx)<0)   
    {
        if(RTC_RetrieveIndex()<0)
            return -1;//�������ɱ��ʧ��
        if(RTC_ReadStartIdx(&_startIdx)<0)
            return -1;//�Ծɴ���
    }
    if(Store_ReadDataItem(_startIdx,_dest)<0)
        return -1;
    if( _startIdx == DATA_MAX_IDX)
        _startIdx = DATA_MIN_IDX;
    else
        ++ _startIdx;
    RTC_SetStartIdx(_startIdx);//����_startIdx;
    return 0;
}
*/

// ���ݵ�ǰ���ö������ݴ�
// ��Ҫ95�ֽڵ�buffer
//  ����ֵΪʵ����д�ĸ���(����д����һ���±�)
//  _dest��дΪ ʱ�䴮*A0000B0000
//  
//  ����������ֱ��ʹ��
//  ��������Ϊ
//  У���ֽ� 0909011230�ֽ�A1�ֽ�A2�ֽ�B1�ֽ�B2�ֽ�..�ֽ�I1�ֽ�I2...�����ֽ�
//  ����:
//  0           1          2          3     
//  0  1234567890 1234567890123456 789012345678  9
//  У 0909011230 AABBCCDDEEFFGGHH IIIJJJKKKLLL ����
// 
//  _buffer(����)
//  0         1         2         3         4
//  01234567890123456789012345678901234567890 
//  0909011230*A4096B4096I0000F0J000000M1N0R1 
int  Store_ReadDataItem(char _index , char * _dest, int _flag)
{
    if( _index < DATA_MIN_IDX || _index > DATA_MAX_IDX)
        return -2;
    int _addr=DATA_START_ADDR + (_index -1)*DATA_ITEM_LEN; //
    char _tempChar1=0x00;
    char _tempChar2=0x00;
    unsigned int  _tempInt=0; 
    char _buffer[50];
    int  _repeats=0;
    int  _idx1=0;//_buffer������
    int  _idx2=0;//_dest������
    int  _read_flag=0;
    while(1)
    {//����3��
        if(_repeats>2)
            return -1;
        //��ȡ����
        //һ�ζ�����.
        //  ÿ��16�ֽ�
        //  DATA_ITEM_LEN = 40
        //
        if(ROM_ReadBytes(_addr,_buffer,16)<0)
            continue;
        if(ROM_ReadBytes(_addr+16,&(_buffer[16]),16)<0)
            continue;
        if(ROM_ReadBytes(_addr+32,&(_buffer[32]),DATA_ITEM_LEN-32)<0)
            continue;
        //������.
        // У�������ڲ���ʹ��,���ֽڱ�������Ϊ ���ͱ��
        //if(Utility_VerifyCrcCode(_buffer,_buffer[0]))
        //    return -3; //У�������ͷ���
        if(_buffer[0]==0x0F)
        {
            _read_flag =1;
        }
        //��дʱ�䴮
        Utility_Strncpy(_dest, &(_buffer[1]),10);
        
        //��λ
        _dest[10]='*';
        _idx2=10;//����ʹ��ǰ׺++
        _idx1=11;
        
        //����ģ��������,����������ѡ����
        if(Store_ReadAnalogSelect(&_tempChar1)<0)
            return -1;
        //���ģ����
        for(int i=0;i<8;++i)
        {
            if(_tempChar1 & 0x01)
            {//�����λ��
                //�����������������
                _dest[++ _idx2] = 'A'+i;
                _tempInt = (((unsigned int)_buffer[_idx1 + 2*i])<<8) + _buffer[_idx1 + 2*i+1];
                
                Utility_UintToStr4(_tempInt,&_dest[_idx2+1]);
                _idx2+=4;
            }
            //Ȼ������һ��
            _tempChar1 >>= 1 ;//����һλ
        }
        
        //���������
        _idx1=27;//
        //��������������,����������ѡ����
        if(Store_ReadPulseSelect(&_tempChar1)<0)
            return -1;
        for(int i=0;i<4;++i)
        {
            if( _tempChar1 & 0x80)
            {//�����λ�в���Ӹ�λ
                _dest[++ _idx2] = 'I' + i ;
                ++ _idx2;
                Utility_CharToHex( _buffer[_idx1+3*i],&(_dest[_idx2]));
                _idx2+=2;//������д2λ.
                Utility_CharToHex( _buffer[_idx1+3*i+1],&(_dest[_idx2]));
                _idx2+=2;
                Utility_CharToHex( _buffer[_idx1+3*i+2],&(_dest[_idx2]));
                _idx2+=1; //��Ϊ����Ĵ�������ž��� ++ _idx2��������ֻ��1.
            }
            _tempChar1<<=1;
        } 
        //������
        _idx1 = 39;
        //_tempChar1װ����������
        //_tempChar2װ������������
        _tempChar1 = _buffer[ _idx1];
        if(Store_ReadIoSelect(&_tempChar2)<0)
            return -1;
        for(int i=0;i<8;++i)
        {//����8��λ
            if(_tempChar2&0x01)
            {//Ϊ1��λҪ��¼0��1
                _dest[++ _idx2] = 'M' + i;
                if(_tempChar1&0x01)
                {   
                    _dest[++ _idx2]='1';
                }
                else
                {
                    _dest[++ _idx2]='0';
                }
            }
            //�ж���һ��
            _tempChar1 >>=1;
            _tempChar2 >>=1;
        }
        //����  ����#��,���ͳ����Լ��� 
        break;
    }
    //���� �ѷ��ͱ�ǽ��з���
    if(_read_flag && !_flag)
        return 1;
    else
        return (_idx2+1); //���ͳ�����Ҫ����λ���ٶ����13.
}
 










