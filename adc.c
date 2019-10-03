//////////////////////////////////////////////////////
//     �ļ���: adc.c
//   �ļ��汾: 2.0.0
//   ����ʱ��: 2010�� 3��4��
//   ��������: ����
//       ����: ����
//       ��ע: ��
// 
//////////////////////////////////////////////////////

#include "msp430common.h"
#include "adc.h"
#include "led.h"
#include "common.h"

#define AD_NUM 8

unsigned int A[8]={0,0,0,0,0,0,0,0}; //  A0Ϊ ��Դ�� 1/2 ��ѹ

void ADC_Open()
{ 
    P6SEL = 0xFF;            //����P6.0~P6.7 Ϊģ������A0~A7 û��
    
    ADC12CTL0 &= ~(ADC12ENC);     //����ENCΪ0���Ӷ��޸�ADC12�Ĵ�������ֵ
    ADC12CTL1 |= ADC12CSTARTADD_0;//ת������ʼ��ַΪ��ADCMEM0	 
    ADC12CTL1 |= ADC12CONSEQ_1;   //ת��ģʽΪ������ͨ��,����ת�� 
    //���ø���ͨ��
    ADC12MCTL0 = ADC12INCH_0 + ADC12SREF0;
    ADC12MCTL1 = ADC12INCH_1 + ADC12SREF0;
    ADC12MCTL2 = ADC12INCH_2 + ADC12SREF0;
    ADC12MCTL3 = ADC12INCH_3 + ADC12SREF0;
    ADC12MCTL4 = ADC12INCH_4 + ADC12SREF0;
    ADC12MCTL5 = ADC12INCH_5 + ADC12SREF0;
    ADC12MCTL6 = ADC12INCH_6 + ADC12SREF0;
    ADC12MCTL7 = ADC12INCH_7 + ADC12SREF0 + ADC12EOS;
  
    ADC12CTL0 |= ADC12ON + ADC12SHT00;    //��ת������
    ADC12CTL0 |= ADC12REFON;      //�򿪲ο���ѹ
    ADC12CTL0 |= ADC12REF2_5V;    //ʹ��2.5V�Ĳο���ѹ
    ADC12CTL0 |= ADC12MSC;        ///* ADC12 Multiple SampleConversion 
  
    ADC12CTL1 |= ADC12SSEL_1; //�õ���ACLK, Խ��Խ��
    ADC12CTL1 |= ADC12DIV_7;  // 8��Ƶ
    ADC12CTL1 |= (ADC12SHP);      //ת��ʱ��������ת����ʱ��
    
    //�������,ʹ��ADת��
    ADC12CTL0 |= ADC12ENC;
    System_Delayms(100); //��ADģ������
    return;
}
int ADC_ReadAnalogStr(int _index, char *_dest)
{//ת������, _index��1��
    if(_index <1 || _index>8)
        return -2;
    int _tempInt = A[--_index];
    Utility_UintToStr4(_tempInt,_dest);
    return 0;
}  
void ADC_Sample()
{
    int _repeats=0;   //ʧ�ܳ��Դ���
    A[0]=0;A[1]=0;A[2]=0;A[3]=0;A[4]=0;A[5]=0;A[6]=0;A[7]=0;//ÿ�β���ǰҪ���A[I]
 
    unsigned int max00=0; 
    unsigned int max10=0; 
    unsigned int max20=0;   
    unsigned int max30=0; 
    unsigned int max40=0; 
    unsigned int max50=0; 
    unsigned int max60=0; 
    unsigned int max70=0; 
    
    unsigned int min00=4096; 
    unsigned int min10=4096; 
    unsigned int min20=4096; 
    unsigned int min30=4096; 
    unsigned int min40=4096; 
    unsigned int min50=4096; 
    unsigned int min60=4096; 
    unsigned int min70=4096; 
    
    unsigned int temp0=0;
    unsigned int temp1=0;
    unsigned int temp2=0;
    unsigned int temp3=0;
    unsigned int temp4=0;
    unsigned int temp5=0;
    unsigned int temp6=0;
    unsigned int temp7=0;   
    //�ܹ�10��, ȥ��1�����ֵ,1����Сֵ,Ȼ�����8.
    //ʵ��ֻɾ��2����ֵ �Բ��Ǻ��� 
    for(int j=0;j<10;++j)
    {
        for(int i=0;i<AD_NUM;++i)
        {
            ADC12CTL0 |= ADC12SC;
            ADC12CTL0 &= ~ADC12SC;
            for(int j=1000;j>1;--j);
        }
        _repeats=0;
        //�ȴ�ת����� 
        while( (ADC12CTL1 & ADC12BUSY)==1)
        {//���ֻ�ȴ�1��
            System_Delayms(100);  
            ++ _repeats;
            if(_repeats>10)
                break;
        }
        temp0=ADC12MEM0; //�������ݼĴ��� �и�����,
        temp1=ADC12MEM1; // �Ȼ��� ����
        temp2=ADC12MEM2;
        temp3=ADC12MEM3;
        temp4=ADC12MEM4;
        temp5=ADC12MEM5;
        temp6=ADC12MEM6;
        temp7=ADC12MEM7; 
        
        //���������Сֵ
        
        if(max00<temp0)  
            max00=temp0; 
        if(min00>temp0) 
            min00=temp0; 
        if(max10<temp1) 
            max10=temp1; 
        if(min10>temp1) 
            min10=temp1; 
        if(max20<temp2) 
            max20=temp2; 
        if(min20>temp2)  
            min20=temp2;  
        if(max30<temp3) 
            max30=temp3; 
        if(min30>temp3) 
            min30=temp3;
        if(max40<temp4)  
            max40=temp4; 
        if(min40>temp4) 
            min40=temp4; 
        if(max50<temp5)  
            max50=temp5; 
        if(min50>temp5) 
            min50=temp5; 
        if(max60<temp6) 
            max60=temp6; 
        if(min60>temp6)  
            min60=temp6; 
        if(max70<temp7)  
            max70=temp7; 
        if(min70>temp7)
            min70=temp7;
        //�ۼ�
        A[0]+=temp0;A[1]+=temp1;A[2]+=temp2;A[3]+=temp3;
        A[4]+=temp4;A[5]+=temp5;A[6]+=temp6;A[7]+=temp7;
    }
    //ȥ�����ֵ ��Сֵ
    A[0]-=(max00 + min00); A[1]-=(max10 + min10);
    A[2]-=(max20 + min20); A[3]-=(max30 + min30);
    A[4]-=(max40 + min40); A[5]-=(max50 + min50);
    A[6]-=(max60 + min60); A[7]-=(max70 + min70);
    //��ƽ����
    A[0]>>=3;A[1]>>=3;A[2]>>=3;A[3]>>=3;A[4]>>=3;A[5]>>=3;A[6]>>=3;A[7]>>=3; 
    
#ifdef __CONSOLE__DEBUG__
    TraceMsg("////////////////////////////////////////",1);
    TraceMsg("A0: ",0);
    TraceInt4(A[0],0);
    TraceMsg("      ",0);
    TraceMsg("A1: ",0);
    TraceInt4(A[1],1);
     
    TraceMsg("A2: ",0);
    TraceInt4(A[2],0);
    TraceMsg("      ",0);
    TraceMsg("A3: ",0);
    TraceInt4(A[3],1);
    
    TraceMsg("A4: ",0);
    TraceInt4(A[4],0);
    TraceMsg("      ",0);
    TraceMsg("A5: ",0);
    TraceInt4(A[5],1);
    
    TraceMsg("A6: ",0);
    TraceInt4(A[6],0);
    TraceMsg("      ",0);
    TraceMsg("A7: ",0);
    TraceInt4(A[7],1);
    TraceMsg("////////////////////////////////////////",1);    
#endif
}

void ADC_Close()
{
    ADC12CTL0 &=~(ADC12ENC);               //��ֹת��
    ADC12CTL0 &=~ ( ADC12ON + ADC12REFON); //�ر�ת������ �� �ڲ��ο���ѹ
}
