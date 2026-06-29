#include "spi1_dma2_manager.h"
#include <stdint.h>
void SPI1_WaitDone(){
    /*Wait for Wait until Transmit buffer empty =1 */
    while (!(SPI1->SR & SPI_SR_TXE)){}
    /*Then wait until Busy flag =0 akak transfer to wire is complete*/
    while (SPI1->SR & SPI_SR_BSY){}

}

void SPI1_Enable(bool enable){
    if (enable){
        SPI1->CR1 |= 1 << SPI_CR1_SPE_Pos;
    }
    else{
        SPI1_WaitDone();
        SPI1->CR1 &= ~(1 << SPI_CR1_SPE_Pos);
    }
} 
void SPI1_Set16Bit(void){
    SPI1_Enable(false);
    SPI1->CR1 |= SPI_CR1_DFF;
    SPI1_Enable(true);

}
void SPI1_Set8Bit(void){
    SPI1_Enable(false);
    SPI1->CR1 &= ~SPI_CR1_DFF;
    SPI1_Enable(true);
}

void SPI1_Write8bit(uint8_t data){
    SPI1_Set8Bit();
    while (!(SPI1->SR & SPI_SR_TXE)){}
    *(__IO uint8_t *)&SPI1->DR = data;

}
void SPI1_Init(void){
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1_Enable(false);
    /*CR1*/

    SPI1->CR1 &= ~SPI_CR1_BIDIMODE_Msk;
    SPI1->CR1 |= 1 << SPI_CR1_BIDIOE_Pos;
    SPI1->CR1 &= ~(1 << SPI_CR1_CRCEN_Pos);
    SPI1->CR1 &= ~SPI_CR1_CRCNEXT;
    SPI1->CR1 &= ~SPI_CR1_RXONLY;
    SPI1->CR1 |= SPI_CR1_SSM;    
    SPI1->CR1 |= SPI_CR1_SSI;    
    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= 1 << SPI_CR1_MSTR;
    SPI1->CR1 &= ~SPI_CR1_CPOL;
    SPI1->CR1 &= ~SPI_CR1_CPHA;
    SPI1->CR1 &= ~SPI_CR1_DFF;
    /*CR2*/
    SPI1->CR2 &= ~SPI_CR2_TXEIE;   
    SPI1->CR2 &= ~SPI_CR2_RXNEIE;  
    SPI1->CR2 &= ~SPI_CR2_ERRIE;   
    SPI1->CR2 &= ~SPI_CR2_FRF;     
    SPI1->CR2 |= SPI_CR2_TXDMAEN;
    SPI1_Enable(true);

}
