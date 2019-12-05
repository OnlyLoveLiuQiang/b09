#include "stm32f10x.h"
#include "key.h"
#include "exti.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
#include "sd.h"
#include "ff.h"
#include "SIM868.h"
#include "gps.h"
#include "spi.h"
#include "mpu9250.h"
#include "bmp280.h"
#include "string.h"

typedef struct{
	unsigned char year[5];
	unsigned char month[3];
	unsigned char day[3];
	unsigned char hour[3];
	unsigned char minute[3];
	unsigned char second[3];
}NPT;

extern char RxBuffer2[1539];
extern int RxCounter2;
extern int gpsDataFlag;

float group_package(unsigned char *data);
void npt_extract(char *buf,NPT *p,UTC *utc);
unsigned char o_finddir(DIR *dirp,FILINFO *fnop,unsigned char *path1,unsigned char *rwpath2,unsigned char *mkpath2);
int bt_function_selection(unsigned char *data);
void clear_btdata(unsigned char *data);
void bt_recevied_data_process(unsigned char *btdata,unsigned char *cmd,unsigned char *data);
void get_list(unsigned char *data);
void get_file(unsigned char *data);
int open_file(FIL *fil,unsigned char *name,float speed);
int close_file(FIL *fil,unsigned char *name,float speed);
int write_file(FIL *fil,unsigned char *data);

unsigned int flightflag;
NPT npt;
char npt_time[48];
unsigned char mode = 0;
unsigned char data[80];
unsigned char btrevdata[32];
unsigned char btcmd[16];
unsigned char btdata[16];
unsigned char reg[16] = "boxV2.0";
unsigned char cpn[16] = "0055C0Q9UOOpI";
unsigned char id[16] = "1234567890123";
char GpsData[1024];
char GPRMC[128];
char GPGGA[128];
GPS_SINGLE_DATA gps_single_data;
UTC tim;
unsigned char path0[3] = "0:";
unsigned char path1[7] = "20";
unsigned char mkpath2[10] = "20";
unsigned char rwpath2[3];
unsigned char name[16] = "20";

int main()
{
	float speed = 0;
	char *bt_recevied_flag = NULL;
	unsigned int i = 0;
	unsigned char gsmmode[] = "GSM Mode";
	unsigned char btmode[] = "BT Mode";
	unsigned int res;
	FATFS fs;
	FIL fil;
	DIR dir0;
	DIR dir1;
	DIR dir2;
	FILINFO fno0;
	FILINFO fno1;
	
	delay_init();
	key_init();
	led_init();
	key_on();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	exti0_init();
	uart1_init(115200);
	uart2_init(115200);
	uart3_init(115200);
	while(sd_init());
	res = f_mount(&fs,"0:",1);
	OPEN_SIM868();
	if(mode == 0){
		Usart_SendStr_length(USART3,gsmmode,8);
		tim.name[0] = '2';
		tim.name[1] = '0';
		ntp_init();
		npt_extract(npt_time,&npt,&tim);
		printf("+++");
		delay_ms(1000);
		GPS_init();
		GSM_init();
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		SIM868_GNSS_UART_ON;
		o_finddir(&dir0,&fno0,path0,path1,path1);
		o_finddir(&dir1,&fno1,path1,rwpath2,mkpath2);
//		f_opendir(&dir2,rwpath2);
//		open_file(&fil,name);
		while(1){
			if(2 == gpsDataFlag){
				gps_buf_extract(RxBuffer2,GpsData,GPGGA,GPRMC);
				//提取数据,提取完成,清除缓存
				for(i=0;i<1539;i++){
					RxBuffer2[i] = '\0';
				}
				RxCounter2 = 0;	
				gpsDataFlag = 0;
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
			}
			Usart_SendStr_length(USART3,(uint8_t *)GpsData,1024);
//			Usart_SendStr_length(USART3,(uint8_t *)GPRMC,128);
//			Usart_SendStr_length(USART3,(uint8_t *)GPGGA,128);	
			speed = group_package(data);
			
			if(flightflag == 0){
				open_file(&fil,tim.name,speed);
			}else if(flightflag == 1){
				write_file(&fil,data);
				close_file(&fil,name,speed);
			}
			
			Usart_SendStr_length(USART3,(uint8_t *)data,79);
			Usart_SendStr_length(USART1,(uint8_t *)data,79);
			delay_ms(1000);
			delay_ms(1000);
		}
	}else if(mode == 1){
		Usart_SendStr_length(USART3,btmode,8);
		BT_init();
		while(BT_connect() == NULL);
		while(1){
			bt_recevied_flag = data_recevied_flag();
			if(NULL != bt_recevied_flag){
				bt_recevied_flag = NULL;
				//提取数据			
				bt_extract_recevied_data(btrevdata);
				bt_recevied_data_process(btrevdata,btcmd,btdata);
				switch(bt_function_selection(btcmd)){
					case 1:	 get_list(btdata); break;
					case 2:  get_file(btdata); break;
					default: bt_send_data((unsigned char *)"enter error\r\n"); break;
				}
//				bt_send_data(btdata);
			}
		}
	}
	while(1);
}

float group_package(unsigned char *data)
{
	float speed = 0;
	
	data_init(data);
	reg_fill(data,reg);
	cpn_fill(data,cpn);
	id_fill(data,id);
	
	//提取经度
	gps_extract(GPRMC,&gps_single_data,5,1);
//	gps_view(&gps_single_data);
	longitude_fill(data,&gps_single_data);
	gps_single_data_clean(&gps_single_data);
	
	//提取纬度
	gps_extract(GPRMC,&gps_single_data,3,1);
//	gps_view(&gps_single_data);
	latitude_fill(data,&gps_single_data);
	gps_single_data_clean(&gps_single_data);
	
	//提取海拔
	gps_extract(GPGGA,&gps_single_data,9,0);
//	gps_view(&gps_single_data);
	elevation_fill(data,&gps_single_data);
	gps_single_data_clean(&gps_single_data);
	
	//提取时间
	time_fill(GPRMC,data,&tim);
	
	//提取地速
	gps_extract(GPRMC,&gps_single_data,7,0);
	speed = speed_fill(data,&gps_single_data);
	gps_single_data_clean(&gps_single_data);
	
	//提取航向
	gps_extract(GPRMC,&gps_single_data,8,0);
//	gps_view(&gps_single_data);
	direction_fill(data,&gps_single_data);
	gps_single_data_clean(&gps_single_data);
	
	other_fill(data,&gps_single_data);
	
//	data_encryption((unsigned char *)data,0xa5,5,77);
	crc_fill((data+3),74);
	
	return speed;
}

void npt_extract(char *buf,NPT *p,UTC *utc)
{
	unsigned char i = 0;
	while('"' != (*buf)) buf = buf + 1;
	buf = buf + 1;
	for(i=0;i<2;i++){
		p->year[i+2] = buf[i];
		path1[i+2] = buf[i];
		mkpath2[i+2] = buf[i];
		name[i+2] = buf[i];
		utc->name[i+2] = buf[i];
	}
	buf = buf + 3;
	for(i=0;i<2;i++){
		p->month[i] = buf[i];
		path1[i+4] = buf[i];
		mkpath2[i+4] = buf[i];
		name[i+4] = buf[i];
		utc->name[i+4] = buf[i];
	}
	mkpath2[6] = '/';
	name[6] = '/';
	utc->name[6] = '/';
	buf = buf + 3;
	for(i=0;i<2;i++){
		p->day[i] = buf[i];
		rwpath2[i] = buf[i];
		mkpath2[i+7] = buf[i];
		name[i+7] = buf[i];
		utc->name[i+7] = buf[i];
	}
	name[9] = '/';
	utc->name[9] = '/';
	buf = buf + 3;
	for(i=0;i<2;i++){
		p->hour[i] = buf[i];
		name[i+10] = buf[i];
	}
	buf = buf + 3;
	for(i=0;i<2;i++){
		p->minute[i] = buf[i];
		name[i+12] = buf[i];
	}
	buf = buf + 3;
	for(i=0;i<2;i++){
		p->second[i] = buf[i];
	}
}

unsigned char o_finddir(DIR *dirp,FILINFO *fnop,unsigned char *path1,unsigned char *rwpath2,unsigned char *mkpath2)
{
	unsigned int res = 0;
	unsigned char pathf = 0;
	res = f_opendir(dirp,(const TCHAR*)path1);
//	printf("f_opendir:res = %d\n",res);
	res = f_readdir(dirp,fnop);
	while(fnop->fname[0] != '\0'){
//		printf("f_readdir:res = %d,fnop->fname = %s\n",res,fnop->fname);
		if(strcmp(fnop->fname,(const char*)rwpath2) == 0){
			pathf = 1;
			break;
		}
		res = f_readdir(dirp,fnop);
	}
	if(pathf != 1){
		res = f_mkdir((const TCHAR*)mkpath2);
//		printf("f_mkdir:res = %d\n",res);
	}
	return pathf;
}

int bt_function_selection(unsigned char *data)
{
	/*
	功能：
	1、读取某一文件夹下文件目录
	2、读取文件(使用蓝牙文件传输可以吗?)
	*/
	if(0 == strcmp((const char *)data,(const char *)"readdir")){
		return 1;
	}
	if(0 == strcmp((const char *)data,(const char *)"getfile")){
		return 2;
	}
	return 0;
}

void clear_btdata(unsigned char *data)
{
	unsigned char i = 0;
	for(i=0;i<32;i++){
		data[i] = 0;
	}
}

void bt_recevied_data_process(unsigned char *btdata,unsigned char *cmd,unsigned char *data)
{
	unsigned char i = 0,j=0;
	while(btdata[i] != ':')
	{
		cmd[i] = btdata[i];
		i++;
	}
	i++;
	while(btdata[i] != 0x0D){
		
		data[j++] = btdata[i++];
	}
}

void get_list(unsigned char *data)
{
	FRESULT res;
	DIR dir;
	FILINFO fno;
	res = f_opendir(&dir,(const TCHAR *)data);
	if(res == FR_OK){
		res = f_readdir(&dir,&fno);
		while(fno.fname[0] != 0){
			bt_send_data((unsigned char *)fno.fname);
			bt_send_data((unsigned char *)"\r\n");
			res = f_readdir(&dir,&fno);
		}
		res = f_closedir(&dir);
	}
}

void get_file(unsigned char *data)
{
	unsigned char rbuff[80];
	UINT rbw;
	FRESULT res;
	FIL fil;
	res = f_open(&fil,(const TCHAR *)data,FA_READ);
	f_sync(&fil);
	if(res == FR_OK){
		res = f_read(&fil,(void *)rbuff,79,&rbw);
		while(rbw != 0){
			res = f_read(&fil,(void *)rbuff,79,&rbw);
			bt_send_data(rbuff);
		}
		res = f_close(&fil);
	}
}

//判断一个架次是否开始
int start(float speed)
{
	static unsigned int flightstart = 0;
	unsigned char c[20] = "\r\nflightstart++\r\n";
	unsigned char d[20] = "\r\nflightstart=0\r\n";
	if(0.0 != speed){
		flightstart++;
		Usart_SendStr_length(USART3,(uint8_t *)c,20);
	}else{
		flightstart = 0;
		Usart_SendStr_length(USART3,(uint8_t *)d,20);
	}
	return flightstart;
}

int stop(float speed)
{
	static unsigned int flightstop = 0;
	unsigned char c[20] = "\r\nflightstop++\r\n";
	unsigned char d[20] = "\r\nflightstop=0\r\n";
	if(0.0 == speed){
		flightstop++;
		Usart_SendStr_length(USART3,(uint8_t *)c,20);
	}else{
		flightstop = 0;
		Usart_SendStr_length(USART3,(uint8_t *)d,20);
	}
	return flightstop;
}

int open_file(FIL *fil,unsigned char *name,float speed)
{
	unsigned char c[20] = "\r\nopen file start\r\n";
	unsigned char d[20] = "\r\nopen file stop\r\n";
	if(start(speed) == 3){
		Usart_SendStr_length(USART3,(uint8_t *)c,20);
		flightflag = 1;
		f_open(fil,(const char*)name,FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		f_sync(fil);
		Usart_SendStr_length(USART3,(uint8_t *)d,20);
	}
	
	return 0;
}

int close_file(FIL *fil,unsigned char *name,float speed)
{
	unsigned char c[20] = "\r\nclose file start\r\n";
	unsigned char d[20] = "\r\nclose file stop\r\n";
	if(stop(speed) == 3){
		Usart_SendStr_length(USART3,(uint8_t *)c,20);
		flightflag = 0;
		f_close(fil);
		Usart_SendStr_length(USART3,(uint8_t *)d,20);
	}
	
	return 0;
}

int write_file(FIL *fil,unsigned char *data)
{
	UINT wbw = 0;
	f_write(fil,(const char *)data,79,&wbw);
	f_sync(fil);
	
	return 0;
}
