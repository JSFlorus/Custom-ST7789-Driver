#ifndef __spi1_dma2_manager_h
#define __spi1_dma2_manager_h
#include <stdint.h>
#include <stdbool.h>
#include "stm32f407xx.h"


void SPI1_Init(void);
void SPI1_Set8Bit(void);
void SPI1_Set16Bit(void);
void SPI1_Write8bit(uint8_t data);
void SPI1_WaitDone(void);
void SPI1_Enable(bool enable);







#endif
