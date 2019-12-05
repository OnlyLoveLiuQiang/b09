#ifndef __SPI_H__
#define __SPI_H__

void spi1_init(void);
void spi1_setSpeed(unsigned char SpeedSet);
unsigned char spi1_readWriteByte(unsigned char TxData);
void spi2_init(void);
unsigned char SPI2_ReadWriteByte(unsigned char TxData);
#endif

