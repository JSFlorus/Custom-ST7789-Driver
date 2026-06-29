#include "dma_manager.h"
#include "spi1_dma2_manager.h"
#include "st7789_manager.h"
#include <stdint.h>
#include <stdint.h>
#include "stm32f407xx.h"
#define DMA_SPI1_MAX_NUM_TRANSFERS 30000


static uint16_t dma_buffers[2][DMA_SPI1_MAX_NUM_TRANSFERS];

DMA_MANAGER_t dma_manager =
{
    .num_transfers = 0,
};



/* Clears Relevant Registers and writes to them */
void Enable_SPI1_DMA2(bool enable){
    if (enable){
        DMA2->LIFCR = DMA_LIFCR_CTCIF3 | DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTEIF3 | DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CFEIF3;
        DMA2_Stream3->CR |= DMA_SxCR_EN;
    }
    else{
        DMA2_Stream3->CR &= ~DMA_SxCR_EN;
        while(DMA2_Stream3->CR &DMA_SxCR_EN){}
    }
}





void Init_SPI1_DMA2(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    SPI1->CR2 |= SPI_CR2_TXDMAEN;
    Enable_SPI1_DMA2(false);



    DMA2->LIFCR = DMA_LIFCR_CTCIF3 | DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTEIF3 | DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CFEIF3;
    /* Use Channel 3*/
    DMA2_Stream3->CR &= ~DMA_SxCR_CHSEL;
    DMA2_Stream3->CR &= ~DMA_SxCR_MBURST;
    DMA2_Stream3->CR &= ~DMA_SxCR_PBURST;
    DMA2_Stream3->CR |= (3U << DMA_SxCR_CHSEL_Pos);
    DMA2_Stream3->CR |= DMA_SxCR_DBM;
    /*Highest Priority*/
    DMA2_Stream3->CR |= (3U << DMA_SxCR_PL_Pos);
    /*Defaul to sending 16bits to SPI1*/
    DMA2_Stream3->CR &= ~DMA_SxCR_PSIZE;
    DMA2_Stream3->CR &= ~DMA_SxCR_MSIZE;
    DMA2_Stream3->CR |= (1U << DMA_SxCR_MSIZE_Pos);
    DMA2_Stream3->CR |= (1U << DMA_SxCR_PSIZE_Pos);
    DMA2_Stream3->CR |= (1U<< DMA_SxCR_MINC_Pos);
    DMA2_Stream3->CR &= ~DMA_SxCR_PINC;
    DMA2_Stream3->CR &= ~DMA_SxCR_DIR;
    DMA2_Stream3->CR |= (1U << DMA_SxCR_DIR_Pos);
    DMA2_Stream3->CR &= ~DMA_SxCR_PFCTRL;
    /*Enable Transfer Complete Interrupt*/
    DMA2_Stream3->CR |= (1U << DMA_SxCR_TCIE_Pos);
    DMA2_Stream3->PAR = 0;
    DMA2_Stream3->M0AR = 0;
    DMA2_Stream3->M1AR = 0;
    DMA2_Stream3->NDTR = 0;
    DMA2_Stream3->PAR = (uint32_t)&SPI1->DR;
    DMA2_Stream3->M0AR = (uint32_t)dma_buffers[0];
    DMA2_Stream3->M1AR = (uint32_t)dma_buffers[1];  
    //DMA2_Stream3->NDTR = dma_manager.max_transfer_size;
    
}


uint8_t DMA_SPI1_GetCurrentTarget(void)
{
    return (DMA2_Stream3->CR & DMA_SxCR_CT) ? 1U : 0U;
}
uint8_t DMA_SPI1_GetWritableTarget(void)
{
    return DMA_SPI1_GetCurrentTarget() ^ 1U;
}
bool DMA_SPI1_GetTransferCompleteFlag(void)
{
    return (DMA2->LISR & DMA_LISR_TCIF3) != 0;
}


bool DMA_SPI1_Write16ToWritableBuffer(uint16_t data)
{
    uint8_t target = DMA_SPI1_GetWritableTarget();

    if (dma_manager.num_transfers > DMA_SPI1_MAX_NUM_TRANSFERS)
    {
        return false;
    }

    uint32_t i = 0;

    while (i < dma_manager.num_transfers)
    {
        dma_buffers[target][i] = data;
        i++;
    }

    return true;
}

bool DMA_SPI1_WriteBufferToWritableBuffer(const uint16_t *data)
{
    if (data == 0)
    {
        return false;
    }

    if (dma_manager.num_transfers == 0U ||
        dma_manager.num_transfers > DMA_SPI1_MAX_NUM_TRANSFERS)
    {
        return false;
    }

    uint8_t target;

    if (DMA_SPI1_IsBusy())
    {

        target = DMA_SPI1_GetWritableTarget();
    }
    else
    {

        target = DMA_SPI1_GetCurrentTarget();
    }

    for (uint32_t i = 0; i < dma_manager.num_transfers; i++)
    {
        dma_buffers[target][i] = data[i];
    }

    return true;
}

void DMA_SPI1_DisableStream(void)
{
    DMA2_Stream3->CR &= ~DMA_SxCR_EN;

    while (DMA2_Stream3->CR & DMA_SxCR_EN)
    {
    }

}
bool DMA_SPI1_SetTransferCount(uint32_t transfers)
{
    if (transfers == 0 || transfers > DMA_SPI1_MAX_NUM_TRANSFERS)
    {
        return false;
    }DMA_SPI1_DisableStream();
    dma_manager.num_transfers = transfers;
    DMA2_Stream3->NDTR = transfers;

    return true;
    DMA2_Stream3->CR |= DMA_SxCR_EN;
}

bool DMA_SPI1_Start(void)
{
    if (DMA_SPI1_IsBusy())
    {
        return false;
    }
    DMA_SPI1_DisableStream();
    DMA2_Stream3->CR |= DMA_SxCR_DBM;

    DMA2_Stream3->M0AR = (uint32_t)dma_buffers[0];
    DMA2_Stream3->M1AR = (uint32_t)dma_buffers[1];

    DMA2_Stream3->NDTR = dma_manager.num_transfers;

    Enable_SPI1_DMA2(true);

    return true;
}


bool DMA_SPI1_IsBusy(void)
{
    if (DMA2_Stream3->CR & DMA_SxCR_EN)
    {
        return true;
    }

    return false;
}

bool DMA_SPI1_Write16ToBuffer(uint8_t target, uint16_t data)
{
    if (target > 1)
    {
        return false;
    }

    if (dma_manager.num_transfers > DMA_SPI1_MAX_NUM_TRANSFERS)
    {
        return false;
    }

    for (uint32_t i = 0; i < dma_manager.num_transfers; i++)
    {
        dma_buffers[target][i] = data;
    }

    return true;
}


void DMA2_Stream3_IRQHandler(void)
{
    if (DMA2->LISR & DMA_LISR_TCIF3)
    {
        DMA2->LIFCR = DMA_LIFCR_CTCIF3;

        DMA_SPI1_DisableStream();
        Enable_SPI1_DMA2(false);
        ST7789_DMACompleteCallback();
    }
}



