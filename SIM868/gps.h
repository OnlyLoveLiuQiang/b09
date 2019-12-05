#ifndef __GPS_H__
#define __GPS_H__ 

typedef struct{
	unsigned char head[4];
	short len;
	unsigned char reg[14];
	unsigned char cpn[14];
	unsigned char id[14];
	int longitude;
	int latitude;
	int elevation;
	unsigned long long time;
	short speed;
	short direction;
	unsigned short precision;
	unsigned char sys_status;
	unsigned char other;
	unsigned char air_status;
	short pitch_angle;
	short roll_angle;
	unsigned short crc;
}GPS_DATA;

typedef struct{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	float second;
	unsigned char name[16];
}UTC;

typedef struct{
	char data[16];
	char flag;		//标识东南西北
	char dataflag;	//标识有无数据
	int len;
	int point;
}GPS_SINGLE_DATA;

#define DEBUG 0
#define LONGITUDE_DEBUG 0
#define UNIX_TIME 1
#define UTC_TIME 0

int gps_extract(char *str,GPS_SINGLE_DATA *gps_single_data_p,int n,int flag);
void gps_buf_extract(char *RxBuffer3_p,char *GpsData_p,char *GPGGA_p,char *GPRMC_p);
void gps_view(GPS_SINGLE_DATA *gps_single_data_p);
void gps_single_data_clean(GPS_SINGLE_DATA *gps_single_data_p);
void data_init(unsigned char *data);
void reg_fill(unsigned char *data,unsigned char *reg);
void cpn_fill(unsigned char *data,unsigned char *cpn);
void id_fill(unsigned char *data,unsigned char *id);
void longitude_fill(unsigned char *data,GPS_SINGLE_DATA *gps_single_data_p);
void latitude_fill(unsigned char *data,GPS_SINGLE_DATA *gps_single_data_p);
void elevation_fill(unsigned char *data,GPS_SINGLE_DATA *gps_single_data_p);
int is_leap_year(int year);
void time_fill(char *gps,unsigned char *data,UTC *time_p);
float speed_fill(unsigned char *data,GPS_SINGLE_DATA *gps_single_data_p);
void direction_fill(unsigned char *data,GPS_SINGLE_DATA *gps_single_data_p);
void other_fill(unsigned char *data,GPS_SINGLE_DATA *gps_single_data_p);
void data_parsing(unsigned char *data,GPS_DATA *gps_data_p);
void data_encryption(unsigned char *data,unsigned char encryption,int begin,int end);
void crc_fill(unsigned char *data,int len);
#endif 
