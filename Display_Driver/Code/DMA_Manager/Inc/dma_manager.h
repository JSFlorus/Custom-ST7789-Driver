#ifndef __DMA_MANAGER_H
#define __DMA_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f407xx.h"









typedef struct
{
    uint32_t num_transfers;
} DMA_MANAGER_t;

extern DMA_MANAGER_t dma_manager;

void Enable_SPI1_DMA2(bool enable);
void Init_SPI1_DMA2(void);
bool DMA_SPI1_GetTransferCompleteFlag(void);
uint8_t DMA_SPI1_GetCurrentTarget(void);
uint8_t DMA_SPI1_GetWritableTarget(void);
bool DMA_SPI1_Write16ToWritableBuffer(uint16_t data);
bool DMA_SPI1_WriteBufferToWritableBuffer(const uint16_t *data);
bool DMA_SPI1_SetTransferCount(uint32_t transfers);
bool DMA_SPI1_Write16ToBuffer(uint8_t target, uint16_t data);
void DMA_SPI1_DisableStream(void);
void DMA2_Stream3_IRQHandler(void);
bool DMA_SPI1_Start(void);
bool DMA_SPI1_IsBusy(void);

#endif
