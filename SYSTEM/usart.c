#include "sys.h"
#include "usart.h"
#include "stdarg.h"
#include "string.h"

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
  	
unsigned char RxBuffer1[UART1_RX_LEN],RxCounter1;
unsigned char TxBuffer1[UART1_RX_LEN],TxCounter1;
  
void uart1_init(u32 bound){
  //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
	  	RxBuffer1[RxCounter1++] = USART_ReceiveData(USART1);
//		USART_SendData(USART3,RxBuffer1[RxCounter1-1]);	
	} 
} 



unsigned char RxBuffer2[UART2_RX_LEN];
int RxCounter2 = 0;
int gpsDataFlag = 0;

void uart2_init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
 	USART_DeInit(USART2);  //��λ����2
	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
		
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

}

void USART2_IRQHandler(void)                	//����2�жϷ������
{
if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //����ģ�鷵�ص�����
	{
		if(RxCounter2 >= UART2_RX_LEN)	RxCounter2=0;
		RxBuffer2[RxCounter2++] =USART_ReceiveData(USART2);//����ģ�������
//		USART_SendData(USART3,RxBuffer2[RxCounter2-1]);
		//���յ�һ����������,�رմ����ж�(���ݴ��������������)
		if(RxCounter2 > 1){
			if('G' == RxBuffer2[RxCounter2-1]){
				if('N' == RxBuffer2[RxCounter2-2]){
					gpsDataFlag++;
					if(2 == gpsDataFlag){
						USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
					}
				}
			}
		}
	} 
}

/*
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		RxBuffer2[1] = USART_ReceiveData(USART2);//����ģ�������
		USART_SendData(USART3,RxBuffer2[1]);	
	}
}
*/

char RxBuffer3[UART3_RX_LEN];
int RxCounter3 = 0;
void uart3_init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ�ܣ�GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//USART3
 	USART_DeInit(USART3);  //��λ����3
	 //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PB10
   
    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��PA3

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure); //��ʼ������
		
//    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 

}

void USART3_IRQHandler(void)                	//����3�жϷ������
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //����ģ�鷵�ص�����
	{
			RxBuffer3[RxCounter3++] = USART_ReceiveData(USART3);//����ģ�������
			USART_SendData(USART3,RxBuffer3[RxCounter3-1]);			
	} 
}

void u_p(char *fmt,...)
{
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char *)TxBuffer1,fmt,ap);
	va_end(ap);
	i = strlen((const char *)TxBuffer1);
	for(j=0;j<i;j++)
	{
		USART_SendData(USART1,TxBuffer1[j]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
	}
	
}

void Uart3_SendStr(char*SendBuf)//����1��ӡ����
{
	while(*SendBuf)
	{
	  while((USART3->SR&0X40)==0);//�ȴ�������� 
		USART3->DR = (u8) *SendBuf; 
		SendBuf++;
	}
}

void Clear_Buffer1(void)//��ջ���
{
		u8 i;
		Uart3_SendStr((char *)RxBuffer1);
		for(i=0;i<RxCounter1;i++)
		RxBuffer1[i]=0;//����
		RxCounter1=0;
//		IWDG_Feed();//ι��
}

void Clear_Buffer2(void)//��ջ���
{
		u8 i;
//		Uart1_SendStr((char *)RxBuffer2);
		for(i=0;i<RxCounter2;i++)
		RxBuffer2[i]=0;//����
		RxCounter2=0;
//		IWDG_Feed();//ι��
}

void Clear_Buffer3(void)//��ջ���
{
		u8 i;
//		Uart1_SendStr((char *)RxBuffer3);
		for(i=0;i<RxCounter3;i++)
		RxBuffer3[i]=0;//����
		RxCounter3=0;
//		IWDG_Feed();//ι��
}

/**********************************�Զ��崮�ں���*****************************************/

/*****************  ����һ���ַ� **********************/
static void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
    /* ����һ���ֽ����ݵ�USART1 */
    USART_SendData(pUSARTx,ch);

    /* �ȴ�������� */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}
/*****************  ָ�����ȵķ����ַ��� **********************/
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen )
{
    unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(k < strlen);
}

