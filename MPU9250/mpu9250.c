#include"mpu9250.h"
#include"stm32f10x_gpio.h"
#include"stm32f10x_spi.h"
#include"delay.h"
#include "spi.h"

void mpu9250_spi2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );
	
	//PB12(SPIx_NSS:软件模式) - 作为mpu9250片选(推挽输出)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

/***************************************************************/
// MPU内部i2c 写入
//I2C_SLVx_ADDR:  MPU9250_AK8963_ADDR
//I2C_SLVx_REG:   reg
//I2C_SLVx_Data out:  value
/***************************************************************/
void i2c_Mag_write(unsigned char reg,unsigned char value)
{
	u16 j=500;
	MPU9250_Write_Reg(I2C_SLV0_ADDR ,MPU9250_AK8963_ADDR);//设置磁力计地址,mode: write
	MPU9250_Write_Reg(I2C_SLV0_REG ,reg);//set reg addr
	MPU9250_Write_Reg(I2C_SLV0_DO ,value);//send value	
	while(j--);//此处因为MPU内部I2C读取速度较慢，延时等待内部写完毕
}

/***************************************************************/
// MPU内部i2c 读取
//I2C_SLVx_ADDR:  MPU9250_AK8963_ADDR
//I2C_SLVx_REG:   reg
//return value:   EXT_SENS_DATA_00 register value
/***************************************************************/
unsigned char i2c_Mag_read(unsigned char reg)
{
	u16 j=5000;
	MPU9250_Write_Reg(I2C_SLV0_ADDR ,MPU9250_AK8963_ADDR|0x80); //设置磁力计地址，mode：read
	MPU9250_Write_Reg(I2C_SLV0_REG ,reg);// set reg addr
	MPU9250_Write_Reg(I2C_SLV0_DO ,0xff);//read
	while(j--);//此处因为MPU内部I2C读取速度较慢，必须延时等待内部读取完毕
	return MPU9250_Read_Reg(EXT_SENS_DATA_00);
}

/***************************************************************/
//SPI发送
//reg: addr
//value:数据
/***************************************************************/
unsigned char MPU9250_Write_Reg(unsigned char reg,unsigned char value)
{
	unsigned char status;
	MPU_9250_ENABLE;   //	MPU9250_CS=0;  //片选MPU9250
	status=SPI2_ReadWriteByte(reg); //发送reg地址
	SPI2_ReadWriteByte(value);//发送数据
	MPU_9250_DISENABLE;//	MPU9250_CS=1;  //失能MPU9250
	return(status);//
}
//---------------------------------------------------------------//
//SPI读取
//reg: addr
unsigned char MPU9250_Read_Reg(unsigned char reg)
{
	u8 reg_val;
	MPU_9250_ENABLE;//	MPU9250_CS=0;  //片选MPU9250
	SPI2_ReadWriteByte(reg|0x80); //reg地址+读命令
	reg_val=SPI2_ReadWriteByte(0xff);//任意数据
	MPU_9250_DISENABLE;//	MPU9250_CS=1;  //失能MPU9250
	return(reg_val);
}

void MPU9250_init(void)
{	
	mpu9250_spi2_init();
	
	MPU9250_Write_Reg(PWR_MGMT_1, 0x80);
	delay_ms(100);
	MPU9250_Write_Reg(PWR_MGMT_1, 0x00);	//解除休眠状态
	MPU9250_Write_Reg(CONFIG, 0x07);      //低通滤波频率，典型值：0x07(3600Hz)此寄存器内决定Internal_Sample_Rate==8K
	
	/**********************Init SLV0 i2c**********************************/	
	//Use SPI-bus read slave0
	MPU9250_Write_Reg(INT_PIN_CFG ,0x30);// INT Pin / Bypass Enable Configuration  
	MPU9250_Write_Reg(I2C_MST_CTRL,0x4d);//I2C MAster mode and Speed 400 kHz
	MPU9250_Write_Reg(USER_CTRL ,0x20); // I2C_MST _EN 
	MPU9250_Write_Reg(I2C_MST_DELAY_CTRL ,0x01);//延时使能I2C_SLV0 _DLY_ enable 	
	MPU9250_Write_Reg(I2C_SLV0_CTRL ,0x81); //enable IIC	and EXT_SENS_DATA==1 Byte
	
	/*******************Init GYRO and ACCEL******************************/	
	MPU9250_Write_Reg(SMPLRT_DIV, 0x07);  //陀螺仪采样率，典型值：0x07(1kHz) (SAMPLE_RATE= Internal_Sample_Rate / (1 + SMPLRT_DIV) )
	MPU9250_Write_Reg(GYRO_CONFIG, 0x18); //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
	MPU9250_Write_Reg(ACCEL_CONFIG, 0x18);//加速计自检、测量范围及高通滤波频率，典型值：0x18(不自检，16G)
	MPU9250_Write_Reg(ACCEL_CONFIG_2, 0x08);//加速计高通滤波频率 典型值 ：0x08  （1.13kHz）	
	/**********************Init MAG **********************************/
	i2c_Mag_write(AK8963_CNTL2_REG,AK8963_CNTL2_SRST); // Reset AK8963
	i2c_Mag_write(AK8963_CNTL1_REG,0x12); // use i2c to set AK8963 working on Continuous measurement mode1 & 16-bit output	
}

void READ_MPU9250_ACCEL(unsigned char *BUF)
{ 

   BUF[0]=MPU9250_Read_Reg(ACCEL_XOUT_L); 
   BUF[1]=MPU9250_Read_Reg(ACCEL_XOUT_H);
	
   BUF[2]=MPU9250_Read_Reg(ACCEL_YOUT_L);
   BUF[3]=MPU9250_Read_Reg(ACCEL_YOUT_H);
	
   BUF[4]=MPU9250_Read_Reg(ACCEL_ZOUT_L); 
   BUF[5]=MPU9250_Read_Reg(ACCEL_ZOUT_H);
}

void READ_MPU9250_GYRO(unsigned char *BUF)
{ 

   BUF[0]=MPU9250_Read_Reg(GYRO_XOUT_L); 
   BUF[1]=MPU9250_Read_Reg(GYRO_XOUT_H);

   BUF[2]=MPU9250_Read_Reg(GYRO_YOUT_L);
   BUF[3]=MPU9250_Read_Reg(GYRO_YOUT_H);
						 
   BUF[4]=MPU9250_Read_Reg(GYRO_ZOUT_L);
   BUF[5]=MPU9250_Read_Reg(GYRO_ZOUT_H);
}

/**********************磁力计读取***************************/
//i2c_Mag_read(AK8963_ST2_REG) 此步读取不可省略
//数据读取结束寄存器，reading this register means data reading end
//AK8963_ST2_REG 同时具有数据非正常溢出检测功能
//详情参考 MPU9250 PDF
/**********************************************************/
void READ_MPU9250_MAG(unsigned char *BUF)
{ 	
	u8 x_axis,y_axis,z_axis; 
	
	x_axis=i2c_Mag_read(AK8963_ASAX);// X轴灵敏度调整值
	y_axis=i2c_Mag_read(AK8963_ASAY);
	z_axis=i2c_Mag_read(AK8963_ASAZ);
	
	if((i2c_Mag_read(AK8963_ST1_REG)&AK8963_ST1_DOR)==0)//data ready
	{
			//读取计算X轴数据
		 BUF[0]=i2c_Mag_read(MAG_XOUT_L); //Low data	
		 if((i2c_Mag_read(AK8963_ST2_REG)&AK8963_ST2_HOFL)==1)// data reading end register & check Magnetic sensor overflow occurred 
		 {
			 BUF[0]=i2c_Mag_read(MAG_XOUT_L);//reload data
		 } 
		 BUF[1]=i2c_Mag_read(MAG_XOUT_H); //High data	
		 if((i2c_Mag_read(AK8963_ST2_REG)&AK8963_ST2_HOFL)==1)// data reading end register
		 {
			 BUF[1]=i2c_Mag_read(MAG_XOUT_H);
		 }
//		 mpu_value.Mag[0]=((BUF[1]<<8)|BUF[0])*(((x_axis-128)>>8)+1);		//灵敏度纠正 公式见/RM-MPU-9250A-00 PDF/ 5.13	
		 
		//读取计算Y轴数据
			BUF[2]=i2c_Mag_read(MAG_YOUT_L); //Low data	
		 if((i2c_Mag_read(AK8963_ST2_REG)&AK8963_ST2_HOFL)==1)// data reading end register
		 {
			 BUF[2]=i2c_Mag_read(MAG_YOUT_L);
		 }		 
		 BUF[3]=i2c_Mag_read(MAG_YOUT_H); //High data	
		 if((i2c_Mag_read(AK8963_ST2_REG)&AK8963_ST2_HOFL)==1)// data reading end register
		 {
			 BUF[3]=i2c_Mag_read(MAG_YOUT_H);
		 }
//		  mpu_value.Mag[1]=((BUF[3]<<8)|BUF[2])*(((y_axis-128)>>8)+1);	
		 
		//读取计算Z轴数据
		 BUF[4]=i2c_Mag_read(MAG_ZOUT_L); //Low data	
		 if((i2c_Mag_read(AK8963_ST2_REG)&AK8963_ST2_HOFL)==1)// data reading end register
		 {
			 BUF[4]=i2c_Mag_read(MAG_ZOUT_L);
		 }	 
		 BUF[5]=i2c_Mag_read(MAG_ZOUT_H); //High data	
		 if((i2c_Mag_read(AK8963_ST2_REG)&AK8963_ST2_HOFL)==1)// data reading end register
		 {
			 BUF[5]=i2c_Mag_read(MAG_ZOUT_H);
		 }
//		  mpu_value.Mag[2]=((BUF[5]<<8)|BUF[4])*(((z_axis-128)>>8)+1);	
	}					       
}
