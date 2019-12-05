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
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//返回OK
	printf("AT\r\n"); 
	delay_ms(300);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//返回OK
	if(strx==NULL)
	{
		CT_SIM868_ON;//拉高
		delay_ms(300);
		delay_ms(300);
		CT_SIM868_OFF;//拉低正常开机
		delay_ms(1000);
		delay_ms(1000);
//		CT_SIM868_ON;
	}
	printf("AT\r\n"); 
	delay_ms(300);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//返回OK
	while(strx==NULL)
	{	
			printf("AT\r\n"); 
			delay_ms(300);
			strx=strstr((const char*)RxBuffer1,(const char*)"OK");//返回OK
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
	strx = strstr((const char*)RxBuffer1,(const char*)"OK");//返回OK
	while(strx==NULL)
	{
		Clear_Buffer1();	
		printf("AT\r\n"); 
		delay_ms(500);
		strx = strstr((const char*)RxBuffer1,(const char*)"OK");//返回OK
	}
	Clear_Buffer1();
	printf("AT+CSQ\r\n");
	delay_ms(500);
	printf(" AT+CREG?\r\n");//查看是否注册GSM网络
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CREG: 0,5");//返回正常
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf(" AT+CREG?\r\n");//查看是否注册GSM网络
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"+CREG: 0,1");//返回正常
	}
	Clear_Buffer1();
	printf(" AT+CGREG?\r\n");//查看是否注册GPRS网络
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CGREG: 0,5");//，这里重要，只有注册成功，才可以进行GPRS数据传输。
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf(" AT+CGREG?\r\n");//查看是否注册GPRS网络
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"+CGREG: 0,1");//，这里重要，只有注册成功，才可以进行GPRS数据传输。
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
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//返回OK
	while(strx==NULL)
	{
		Clear_Buffer1();	
		printf("AT\r\n"); 
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//返回OK
	}
//	printf("ATE0\r\n"); //打开回显
//	delay_ms(500);
//	for(i=0;i<255;i++){
//	Clear_Buffer1();	
//	printf("AT+CSQ\r\n"); //检查CSQ，可以利用手机APP查看CSQ值
//	delay_ms(500);
//	}
	/////////////////////////////////
	printf("AT+CPIN?\r\n");//检查SIM卡是否在位
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CPIN: READY");//查看是否返回ready
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CPIN?\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"+CPIN: READY");//检查SIM卡是否在位，等待卡在位，如果卡识别不到，剩余的工作就没法做了
	}
	Clear_Buffer1();
	printf("AT+CCID\r\n");
	Clear_Buffer1();
	///////////////////////////////////
	printf(" AT+CREG?\r\n");//查看是否注册GSM网络
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CREG: 0,5");//返回正常
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf(" AT+CREG?\r\n");//查看是否注册GSM网络
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"+CREG: 0,1");//返回正常
	}
	Clear_Buffer1();
	/////////////////////////////////////
	printf(" AT+CGREG?\r\n");//查看是否注册GPRS网络
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CGREG: 0,5");//，这里重要，只有注册成功，才可以进行GPRS数据传输。
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf(" AT+CGREG?\r\n");//查看是否注册GPRS网络
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"+CGREG: 0,1");//，这里重要，只有注册成功，才可以进行GPRS数据传输。
	}
	Clear_Buffer1();
	printf("AT+CIPCLOSE=1\r\n");//关闭TCP连接，防止前面的连接没有断开
	delay_ms(500);
	printf("AT+CIPSHUT\r\n");//关闭移动场景
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"SHUT OK");//关闭成功
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CIPSHUT\r\n");//关闭移动场景
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"SHUT OK");//关闭成功
	}
	Clear_Buffer1();
	printf("AT+CGCLASS=\"B\"\r\n");//设置GPRS移动台类别为B,支持包交换和数据交换 
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//开启成功
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CGCLASS=\"B\"\r\n");//设置GPRS移动台类别为B,支持包交换和数据交换 
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");////开启成功
	}
	Clear_Buffer1();
	printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");//设置PDP上下文,互联网接协议,接入点等信息 APN
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//设置OK
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");//设置PDP上下文,互联网接协议,接入点等信息 APN
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//开启成功
	}
	Clear_Buffer1();
	printf("AT+CGATT=1\r\n");//附着GPRS业务
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//开启成功
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CGATT=1\r\n");//附着GPRS业务
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//开启成功
	}		
	Clear_Buffer1();
	
	printf("AT+CIPCSGP=1,\"CMNET\"\r\n");//设置为GPRS连接模式
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//设置ok
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CIPCSGP=1,\"CMNET\"\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//设置ok
	}
	Clear_Buffer1();
	printf("AT+CIPMUX=0\r\n");//单路传输
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//设置ok
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CIPMUX=0\r\n");
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//设置ok
	}
	Clear_Buffer1();	
	printf("AT+CIPMODE=1\r\n");//打开透传功能
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//设置ok
	while(strx==NULL)
	{
		Clear_Buffer1();
		printf("AT+CIPMODE=1\r\n");//打开透传功能
		delay_ms(500);
		strx=strstr((const char*)RxBuffer1,(const char*)"OK");//设置ok
	}
	Clear_Buffer1();
//	printf("AT+CIPSTART=\"TCP\",\"242e119n19.qicp.vip\",17523\r\n");//这里是需要登陆的IP号码，输入公网的IP以及端口号就可以了。
//	printf("AT+CIPSTART=\"TCP\",\"223.100.24.135\",13000\r\n");
	printf("AT+CIPSTART=\"TCP\",\"47.92.81.218\",1194\r\n");
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"OK");//检查是否登陆成功
	while(strx==NULL&&Readystrx==NULL)
	{
		strx=strstr((const char*)RxBuffer1,(const char*)"CONNECT OK");//判断连接状态
		Readystrx=strstr((const char*)RxBuffer1,(const char*)"ALREADY CONNECT"); //已经连接状态
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
	printf("AT+CGPSPWR?\r\n"); //查看是否上电了
	delay_ms(500);
	strx=strstr((const char*)RxBuffer1,(const char*)"+CGPSPWR: 0");//返回0，需要重新上电
	if(strx)
	{
		printf("AT+CGPSPWR=1\r\n"); //对模块进行上电
		delay_ms(500);
	}
	Clear_Buffer1();	
	printf("AT+CGPSSTATUS?\r\n"); //查看定位状态
	delay_ms(500);
	Clear_Buffer1();	
	printf("AT+CGPSOUT=0\r\n");//开启数据获取
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
	strx=strstr((const char*)RxBuffer1,(const char*)"+BTPAIRING:");//获取到有连接对象
	if(strx)
	{
		printf("AT+BTPAIR=1,1\r\n");//允许连接
		delay_ms(500);
		Clear_Buffer1();	
    }
	strx = NULL;
	strx=strstr((const char*)RxBuffer1,(const char*)"+BTCONNECTING:");//密码配对ok
	if(strx)
	{
		printf("AT+BTACPT=1\r\n");//正式连接
		delay_ms(500);
		Clear_Buffer1();	
    }
	strx = NULL;
	strx=strstr((const char*)RxBuffer1,(const char*)"+BTCONNECT:");//连接成功返回
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
		if(j==2)//第二个逗号了，后面跟的是数据
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
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
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
