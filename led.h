//////////////////////////////////////////////////////
//     �ļ���: led.h
//   �ļ��汾: 1.0.0  
//   ����ʱ��: 09��11��30��
//   ��������: �ޡ� 
//       ����: ����
//       ��ע: 
// 
//
//////////////////////////////////////////////////////


#pragma once

//
//    LED1   p1.7  //ԭ��ͼ D7    
//    LED3   p4.7  //ԭ��ͼ D3
//    LED4   p4.3  //ԭ��ͼ D4
//

//
//   led1  75ms�Ŀ��� ��ʾһ��ϵͳ�ĵδ�Ticks   (��ʱ��A���ж� )
//   led1  ������3����˸ ��ʾ�յ���������.
//   ....
//   ... 
//   led4  ������3����˸ ��ʾϵͳ����
//    
 
void Led_Init();
void Led1_On();
void Led2_On();
void Led3_On();
void Led4_On();
void Led5_On();
void Led6_On();
void Led1_Off();
void Led2_Off();
void Led3_Off();
void Led4_Off(); 
void Led5_Off(); 
void Led6_Off(); 

void Led_Round(); 
void Led_LongOn(); 

void Led_WARN();
void Led1_WARN();
void Led2_WARN();
void Led3_WARN();
void Led4_WARN();
 
void Led_Flash();
 

