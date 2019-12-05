#include"SIM868.h"
#include"usart.h"
#include"delay.h"
#include"sys.h"
#include"string.h"

char *Readystrx;
unsigned char getdata[100];
extern unsigned char RxCounter1;
extern char npt_time[48];

static void pa5_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

static void pb0_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
//	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);
}

static void pb1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
}

void OPEN_SIM868(void)
{
	char *strx = NULL;
	pa5_init();
	pb0_init();
	pb1_init();
	printf("AT\r\n"); 
	delay_ms(300);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����OK
	printf("AT\r\n"); 
	delay_ms(300);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����OK
	if(strx==NULL)
	{
		CT_SIM868_ON;//����
		delay_ms(300);
		delay_ms(300);
		CT_SIM868_OFF;//������������
		delay_ms(1000);
		delay_ms(1000);
//		CT_SIM868_ON;
	}
	printf("AT\r\n"); 
	delay_ms(300);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����OK
	while(strx==NULL)
	{	
			printf("AT\r\n"); 
			delay_ms(300);
			strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����OK
	}
	Clear_Buffer1();
	printf("ATE1\r\n");
	delay_ms(500);
	Clear_Buffer1();
//	printf("AT+CBC\r\n");
//	delay_ms(300);
//	Clear_Buffer1();
}

void ntp_init(void)
{
	unsigned char i = 0;
	char *strx = NULL;
	printf("AT\r\n"); 
	delay_ms(500);
	printf("AT\r\n"); 
	delay_ms(500);
	strx = strstr((const char*)RxBuffer1,(const char*)"OK");//����OK
	while(strx==NULL)
	{
		Clear_Buffer1();	
		printf("AT\r\n"); 
		delay_ms(500);
		strx = strstr((const char*)RxBuffer1,(const char*)"OK");//����OK
	}
	Clear_Buffer1();
	printf("AT+CSQ\r\n");
	delay_ms(500);
	printf(" AT+CREG?\r\n");//�鿴�Ƿ�ע��GSM����
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CREG: 0,5");//��������
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf(" AT+CREG?\r\n");//�鿴�Ƿ�ע��GSM����
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"+CREG: 0,1");//��������
	}
	Clear_Buffer1();
	printf(" AT+CGREG?\r\n");//�鿴�Ƿ�ע��GPRS����
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CGREG: 0,5");//��������Ҫ��ֻ��ע��ɹ����ſ��Խ���GPRS���ݴ��䡣
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf(" AT+CGREG?\r\n");//�鿴�Ƿ�ע��GPRS����
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"+CGREG: 0,1");//��������Ҫ��ֻ��ע��ɹ����ſ��Խ���GPRS���ݴ��䡣
	}
	Clear_Buffer1();
	printf("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n");
	delay_ms(500);
	strx = strstr((const char *)RxBuffer1,(const char *)"OK");
	while(strx==NULL){
		Clear_Buffer1();
		printf("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n");
		delay_ms(500);
		strx = strstr((const char *)RxBuffer1,(const char *)"OK");
	}
	Clear_Buffer1();
	printf("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n");
	delay_ms(500);
	strx = strstr((const char *)RxBuffer1,(const char *)"OK");
	while(strx==NULL){
		Clear_Buffer1();
		printf("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n");
		delay_ms(500);
		strx = strstr((const char *)RxBuffer1,(const char *)"OK");
	}
	Clear_Buffer1();
	printf("AT+SAPBR=1,1\r\n");
	delay_ms(500);
	strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	while(strx==NULL){
		Clear_Buffer1();
		printf("AT+SAPBR=1,1\r\n");
		delay_ms(500);
		strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+SAPBR=2,1\r\n");
	delay_ms(500);
	strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	while(strx==NULL){
		Clear_Buffer1();
		printf("AT+SAPBR=2,1\r\n");
		delay_ms(500);
		strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+CNTPCID=1\r\n");
	delay_ms(500);
	strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	while(strx==NULL){
		Clear_Buffer1();
		printf("AT+CNTPCID=1\r\n");
		delay_ms(500);
		strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+CNTP=\"ntp2.aliyun.com\",32\r\n");
	delay_ms(500);
	strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	while(strx==NULL){
		Clear_Buffer1();
		printf("AT+CNTP=\"ntp2.aliyun.com\",32\r\n");
		delay_ms(500);
		strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+CNTP?\r\n");
	delay_ms(500);
	strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	while(strx==NULL){
		Clear_Buffer1();
		printf("AT+CNTP?\r\n");
		delay_ms(500);
		strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+CNTP\r\n");
	delay_ms(500);
	strx = strstr((const char*)RxBuffer1,(const char*)"CNTP: 1");
	while(strx==NULL){
		Clear_Buffer1();
		printf("AT+CNTP\r\n");
		delay_ms(500);
		strx = strstr((const char*)RxBuffer1,(const char*)"CNTP: 1");
	}
	Clear_Buffer1();
	printf("AT+CCLK?\r\n");
	delay_ms(500);
	strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	while(strx==NULL){
		Clear_Buffer1();
		printf("AT+CCLK?\r\n");
		delay_ms(500);
		strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	strx = NULL;
	strx = strstr((const char*)RxBuffer1,(const char*)"+CCLK");
	while(strx[i]!=0){
		npt_time[i] = strx[i];
		i++;
	}
	Clear_Buffer1();
	
	agps_init();
	
	printf("AT+SAPBR=0,1\r\n");
	delay_ms(500);
	strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	while(strx==NULL){
		Clear_Buffer1();
		printf("AT+SAPBR=0,1\r\n");
		delay_ms(500);
		strx = strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
}

void  GSM_init(void)
{
//	unsigned char i = 0;
	char *strx = NULL;
	printf("AT\r\n"); 
	delay_ms(500);
	printf("AT\r\n"); 
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����OK
	while(strx==NULL)
	{
		Clear_Buffer1();	
		printf("AT\r\n"); 
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����OK
	}
//	printf("ATE0\r\n"); //�򿪻���
//	delay_ms(500);
//	for(i=0;i<255;i++){
//	Clear_Buffer1();	
//	printf("AT+CSQ\r\n"); //���CSQ�����������ֻ�APP�鿴CSQֵ
//	delay_ms(500);
//	}
	/////////////////////////////////
	printf("AT+CPIN?\r\n");//���SIM���Ƿ���λ
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CPIN: READY");//�鿴�Ƿ񷵻�ready
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CPIN?\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"+CPIN: READY");//���SIM���Ƿ���λ���ȴ�����λ�������ʶ�𲻵���ʣ��Ĺ�����û������
	}
	Clear_Buffer1();
	printf("AT+CCID\r\n");
	Clear_Buffer1();
	///////////////////////////////////
	printf(" AT+CREG?\r\n");//�鿴�Ƿ�ע��GSM����
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CREG: 0,5");//��������
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf(" AT+CREG?\r\n");//�鿴�Ƿ�ע��GSM����
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"+CREG: 0,1");//��������
	}
	Clear_Buffer1();
	/////////////////////////////////////
	printf(" AT+CGREG?\r\n");//�鿴�Ƿ�ע��GPRS����
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CGREG: 0,5");//��������Ҫ��ֻ��ע��ɹ����ſ��Խ���GPRS���ݴ��䡣
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf(" AT+CGREG?\r\n");//�鿴�Ƿ�ע��GPRS����
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"+CGREG: 0,1");//��������Ҫ��ֻ��ע��ɹ����ſ��Խ���GPRS���ݴ��䡣
	}
	Clear_Buffer1();
	printf("AT+CIPCLOSE=1\r\n");//�ر�TCP���ӣ���ֹǰ�������û�жϿ�
	delay_ms(500);
	printf("AT+CIPSHUT\r\n");//�ر��ƶ�����
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"SHUT OK");//�رճɹ�
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CIPSHUT\r\n");//�ر��ƶ�����
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"SHUT OK");//�رճɹ�
	}
	Clear_Buffer1();
	printf("AT+CGCLASS=\"B\"\r\n");//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ��� 
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//�����ɹ�
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CGCLASS=\"B\"\r\n");//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ��� 
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");////�����ɹ�
	}
	Clear_Buffer1();
	printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");//����PDP������,��������Э��,��������Ϣ APN
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����OK
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");//����PDP������,��������Э��,��������Ϣ APN
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//�����ɹ�
	}
	Clear_Buffer1();
	printf("AT+CGATT=1\r\n");//����GPRSҵ��
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//�����ɹ�
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CGATT=1\r\n");//����GPRSҵ��
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//�����ɹ�
	}		
	Clear_Buffer1();
	
	printf("AT+CIPCSGP=1,\"CMNET\"\r\n");//����ΪGPRS����ģʽ
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����ok
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CIPCSGP=1,\"CMNET\"\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����ok
	}
	Clear_Buffer1();
	printf("AT+CIPMUX=0\r\n");//��·����
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����ok
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CIPMUX=0\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����ok
	}
	Clear_Buffer1();	
	printf("AT+CIPMODE=1\r\n");//��͸������
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����ok
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CIPMODE=1\r\n");//��͸������
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����ok
	}
	Clear_Buffer1();
//	printf("AT+CIPSTART=\"TCP\",\"242e119n19.qicp.vip\",17523\r\n");//��������Ҫ��½��IP���룬���빫����IP�Լ��˿ںžͿ����ˡ�
//	printf("AT+CIPSTART=\"TCP\",\"223.100.24.135\",13000\r\n");
	printf("AT+CIPSTART=\"TCP\",\"47.92.81.218\",1194\r\n");
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//����Ƿ��½�ɹ�
	while(strx==NULL&&Readystrx==NULL)
	{
		strx=strstr((const char*)RxBuffer1,(const char*)"CONNECT OK");//�ж�����״̬
		Readystrx=strstr((const char*)RxBuffer1,(const char*)"ALREADY CONNECT"); //�Ѿ�����״̬
		delay_ms(100);
	}
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	Clear_Buffer1();
}

void  GPS_init(void)
{	
	char *strx = NULL;
	Clear_Buffer1();
	printf("AT+CGPSPWR?\r\n"); //�鿴�Ƿ��ϵ���
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CGPSPWR: 0");//����0����Ҫ�����ϵ�
	if(strx)
	{
		printf("AT+CGPSPWR=1\r\n"); //��ģ������ϵ�
		delay_ms(500);
	}
	Clear_Buffer1();	
	printf("AT+CGPSSTATUS?\r\n"); //�鿴��λ״̬
	delay_ms(500);
	Clear_Buffer1();	
	printf("AT+CGPSOUT=0\r\n");//�������ݻ�ȡ
}	

void BT_init(void)
{
	printf("AT+BTPOWER=1\r\n");
	delay_ms(1000);
	Clear_Buffer1();
	printf("AT+BTHOST=5uCloud\r\n");
	delay_ms(1000);
	Clear_Buffer1();
	printf("AT+BTHOST?\r\n");
	delay_ms(1000);
	Clear_Buffer1();
	printf("AT+BTVIS=1\r\n");
	delay_ms(1000);
	Clear_Buffer1();
}

char *BT_connect(void)
{
	unsigned char i = 0;
	char *strx = NULL;
	strx=strstr((const char*)RxBuffer1,(const char*)"+BTPAIRING:");//��ȡ�������Ӷ���
	if(strx)
	{
		printf("AT+BTPAIR=1,1\r\n");//��������
		delay_ms(500);
		Clear_Buffer1();	
    }
	strx = NULL;
	strx=strstr((const char*)RxBuffer1,(const char*)"+BTCONNECTING:");//�������ok
	if(strx)
	{
		printf("AT+BTACPT=1\r\n");//��ʽ����
		delay_ms(500);
		Clear_Buffer1();	
    }
	strx = NULL;
	strx=strstr((const char*)RxBuffer1,(const char*)"+BTCONNECT:");//���ӳɹ�����
	delay_ms(1000);
	if(strx){
		for(i=0;i<RxCounter1;i++){
			RxBuffer1[i] = 0;
		}
		RxCounter1 = 0;
	}
	return strx;
}

char *data_recevied_flag(void)
{
	char *strx = NULL;
	strx=strstr((const char*)RxBuffer1,(const char*)"+BTSPPDATA:");
	delay_ms(1000);
	return strx;
}

void bt_extract_recevied_data(unsigned char *data)
{
	unsigned char i=0,j=0,k=0;
	for(i=0;i<RxCounter1;i++)
	{
		if(RxBuffer1[i]==',')
		j++;
		if(j==2)//�ڶ��������ˣ��������������
		 {
			k=i;
			break;
		 }
	}
	for(i=k+1;i<RxCounter1;i++) data[i-k-1]=RxBuffer1[i];
	data[i-k-1]=0;
	Clear_Buffer1();
}

void bt_send_data(unsigned char *data)
{
	printf("AT+BTSPPSEND\r\n");
	delay_ms(100);
	Usart_SendStr_length(USART1,(unsigned char *)data,79);
	delay_ms(100);
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) 0x1a;   
	delay_ms(100);
	Clear_Buffer1();
}

void agps_init(void)
{
	char *strx = NULL;
	printf("AT+CLBS=1,1\r\n");
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CLBS: 0");
	while(strx == NULL){
		Clear_Buffer1();
		printf("AT+CLBS=1,1\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"+CLBS: 0");
	}
	Clear_Buffer1();
	printf("AT+CGNSSAV=3,3\r\n");
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	if(strx == NULL){
		Clear_Buffer1();
		printf("AT+CGNSSAV=3,3\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+HTTPINIT\r\n");
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	if(strx == NULL){
		Clear_Buffer1();
		printf("AT+HTTPINIT\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+HTTPPARA=\"CID\",1\r\n");
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	if(strx == NULL){
		Clear_Buffer1();
		printf("AT+HTTPPARA=\"CID\",1\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+HTTPPARA=\"URL\",\"http://wepodownload.mediatek.com/EPO_GPS_3_1.DAT\"\r\n");
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	if(strx == NULL){
		Clear_Buffer1();
		printf("AT+HTTPPARA=\"URL\",\"http://wepodownload.mediatek.com/EPO_GPS_3_1.DAT\"\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+HTTPACTION=0\r\n");
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	if(strx == NULL){
		Clear_Buffer1();
		printf("AT+HTTPACTION=0\r\n");
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+HTTPTERM\r\n");
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	if(strx == NULL){
		Clear_Buffer1();
		printf("AT+HTTPTERM\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+FSLS=C:\\User\\\r\n");
	delay_ms(1000);
	delay_ms(1000);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	if(strx == NULL){
		Clear_Buffer1();
		printf("AT+FSLS=C:\\User\\\r\n");
		delay_ms(1000);
		delay_ms(1000);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+CGNSCHK=3,1\r\n");
	delay_ms(1000);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	if(strx == NULL){
		Clear_Buffer1();
		printf("AT+CGNSCHK=3,1\r\n");
		delay_ms(1000);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+CGNSPWR=1\r\n");
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	if(strx == NULL){
		Clear_Buffer1();
		printf("AT+CGNSPWR=1\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+CGNSAID=31,1,1\r\n");
	delay_ms(1000);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	if(strx == NULL){
		Clear_Buffer1();
		printf("AT+CGNSAID=31,1,1\r\n");
		delay_ms(1000);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");
	}
	Clear_Buffer1();
	printf("AT+CGNSINF\r\n");
	delay_ms(500);
	Clear_Buffer1();
}
