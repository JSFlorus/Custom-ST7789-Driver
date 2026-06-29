#include "st7789_manager.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32f407xx.h"
#include "spi1_dma2_manager.h"
#include "dma_manager.h"
#include "main.h"
ST7789_Config st7789 =
{
    .height = 320,
    .width = 240,

    .x_offset = 0,
    .y_offset = 0,
    .frame_rate = ST7789_FRAME_RATE_120HZ,

    .color_mode = ST7789_COLOR_RGB565,
    .orientation = ST7789_ORIENTATION_0,
};



void ST7789_Init(void)
{
    //Based on Waveshare Defaults
    ST7789_Reset();
    ST7789_WriteCommand(ST7789_SWRESET);
    HAL_Delay(150);
    ST7789_WriteCommand(ST7789_SLPOUT);
    HAL_Delay(120);
    ST7789_ApplyConfiguration();
    ST7789_WriteCommand(ST7789_PORCTRL);
    ST7789_WriteData_Byte(0x0C);
    ST7789_WriteData_Byte(0x0C);
    ST7789_WriteData_Byte(0x00);
    ST7789_WriteData_Byte(0x33);
    ST7789_WriteData_Byte(0x33);
    ST7789_WriteCommand(ST7789_GCTRL);
    ST7789_WriteData_Byte(0x35);
    ST7789_WriteCommand(ST7789_VCOMS);
    ST7789_WriteData_Byte(0x1F);
    ST7789_WriteCommand(ST7789_LCMCTRL);
    ST7789_WriteData_Byte(0x2C);
    ST7789_WriteCommand(ST7789_VDVVRHEN);
    ST7789_WriteData_Byte(0x01);
    ST7789_WriteCommand(ST7789_VRHS);
    ST7789_WriteData_Byte(0x12);
    ST7789_WriteCommand(ST7789_VDVS);
    ST7789_WriteData_Byte(0x20);
    ST7789_WriteCommand(ST7789_PWCTRL1);
    ST7789_WriteData_Byte(0xA4);
    ST7789_WriteData_Byte(0xA1);
    ST7789_WriteCommand(ST7789_PVGAMCTRL);
    ST7789_WriteData_Byte(0xD0);
    ST7789_WriteData_Byte(0x08);
    ST7789_WriteData_Byte(0x11);
    ST7789_WriteData_Byte(0x08);
    ST7789_WriteData_Byte(0x0C);
    ST7789_WriteData_Byte(0x15);
    ST7789_WriteData_Byte(0x39);
    ST7789_WriteData_Byte(0x33);
    ST7789_WriteData_Byte(0x50);
    ST7789_WriteData_Byte(0x36);
    ST7789_WriteData_Byte(0x13);
    ST7789_WriteData_Byte(0x14);
    ST7789_WriteData_Byte(0x29);
    ST7789_WriteData_Byte(0x2D);
    ST7789_WriteCommand(ST7789_NVGAMCTRL);
    ST7789_WriteData_Byte(0xD0);
    ST7789_WriteData_Byte(0x08);
    ST7789_WriteData_Byte(0x10);
    ST7789_WriteData_Byte(0x08);
    ST7789_WriteData_Byte(0x06);
    ST7789_WriteData_Byte(0x06);
    ST7789_WriteData_Byte(0x39);
    ST7789_WriteData_Byte(0x44);
    ST7789_WriteData_Byte(0x51);
    ST7789_WriteData_Byte(0x0B);
    ST7789_WriteData_Byte(0x16);
    ST7789_WriteData_Byte(0x14);
    ST7789_WriteData_Byte(0x2F);
    ST7789_WriteData_Byte(0x31);
    ST7789_WriteCommand(ST7789_INVON);
    ST7789_WriteCommand(ST7789_NORON);
    HAL_Delay(10);
    ST7789_WriteCommand(ST7789_DISPON);
    HAL_Delay(100);

}

void ST7789_Select(void)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
}

void ST7789_Unselect(void)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void ST7789_SetCommandMode(void)
{
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
}

void ST7789_SetDataMode(void)
{
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
}

void ST7789_WriteCommand(uint8_t cmd)
{
    ST7789_Select();
    ST7789_SetCommandMode();
    SPI1_Write8bit(cmd);
    SPI1_WaitDone();
    ST7789_Unselect();
}

void ST7789_WriteData_Byte(uint8_t data)
{
    ST7789_Select();
    ST7789_SetDataMode();
    SPI1_Write8bit(data);
    SPI1_WaitDone();
    ST7789_Unselect();
}


void ST7789_Reset(void)
{
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);

    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
}


 


static uint8_t ST7789_FrameRateToRegister(ST7789_FrameRate frameRate)
{
    switch (frameRate)
    {
        case ST7789_FRAME_RATE_45HZ:  return 0x19;
        case ST7789_FRAME_RATE_60HZ:  return 0x0F;
        case ST7789_FRAME_RATE_75HZ:  return 0x09;
        case ST7789_FRAME_RATE_90HZ:  return 0x05;
        case ST7789_FRAME_RATE_120HZ: return 0x00;

        default:                     return 0x0F; // 60 Hz
    }
}

void ST7789_SetFrameRate(ST7789_FrameRate frameRate)
{
    st7789.frame_rate = frameRate;

    ST7789_WriteCommand(ST7789_FRCTRL2);
    ST7789_WriteData_Byte(ST7789_FrameRateToRegister(frameRate));
}




static uint8_t ST7789_ColorModeToRegister(ST7789_ColorMode colorMode)
{
    switch (colorMode)
    {
        case ST7789_COLOR_RGB444:
            return 0x03;

        case ST7789_COLOR_RGB565:
            return 0x05;

        case ST7789_COLOR_RGB666:
            return 0x06;

        default:
            return 0x05;    // Default to RGB565
    }
}
static uint8_t ST7789_OrientationToRegister(ST7789_Orientation orientation)
{
    switch (orientation)
    {
        case ST7789_ORIENTATION_0:
            return 0x00;

        case ST7789_ORIENTATION_90:
            return 0x60;

        case ST7789_ORIENTATION_180:
            return 0xC0;

        case ST7789_ORIENTATION_270:
            return 0xA0;

        default:
            return 0x00;
    }
}

void ST7789_SetWindow(uint16_t x,
                      uint16_t y,
                      uint16_t width,
                      uint16_t height)
{
    uint16_t startColumn = x + st7789.x_offset;
    uint16_t endColumn   = startColumn + width - 1;

    uint16_t startRow = y + st7789.y_offset;
    uint16_t endRow   = startRow + height - 1;

    ST7789_WriteCommand(ST7789_CASET);
    ST7789_WriteData_Byte(startColumn >> 8);
    ST7789_WriteData_Byte(startColumn & 0xFF);
    ST7789_WriteData_Byte(endColumn >> 8);
    ST7789_WriteData_Byte(endColumn & 0xFF);

    ST7789_WriteCommand(ST7789_RASET);
    ST7789_WriteData_Byte(startRow >> 8);
    ST7789_WriteData_Byte(startRow & 0xFF);
    ST7789_WriteData_Byte(endRow >> 8);
    ST7789_WriteData_Byte(endRow & 0xFF);
}


void ST7789_ApplyConfiguration(void)
{

    ST7789_SetOrientation(st7789.orientation);
    ST7789_SetColorMode(st7789.color_mode);
    ST7789_SetFrameRate(st7789.frame_rate);
}

void ST7789_SetOrientation(ST7789_Orientation orientation)
{
    st7789.orientation = orientation;

    ST7789_WriteCommand(ST7789_MADCTL);
    ST7789_WriteData_Byte(ST7789_OrientationToRegister(orientation));
}

void ST7789_StartWrite(void)
{
    ST7789_WriteCommand(ST7789_RAMWR);
}
void ST7789_SetColorMode(ST7789_ColorMode colorMode)
{
    st7789.color_mode = colorMode;

    ST7789_WriteCommand(ST7789_COLMOD);
    ST7789_WriteData_Byte(ST7789_ColorModeToRegister(colorMode));

}


bool ST7789_Update(void)
{
    return ST7789_UpdateWindow(0, 0, st7789.width, st7789.height);
}

bool ST7789_UpdateWindow(uint16_t x,
                         uint16_t y,
                         uint16_t width,
                         uint16_t height)
{
    uint32_t pixel_count = (uint32_t)width * height;

    if (width == 0 || height == 0)
    {
        return false;
    }

    if (x + width > st7789.width || y + height > st7789.height)
    {
        return false;
    }

    if (DMA_SPI1_IsBusy())
    {
        return false;
    }

    if (!DMA_SPI1_SetTransferCount(pixel_count))
    {
        return false;
    }

    ST7789_SetWindow(x, y, width, height);
    ST7789_StartWrite();

    ST7789_Select();
    ST7789_SetDataMode();

    SPI1_Set16Bit();

    return DMA_SPI1_Start();
}

void ST7789_DMACompleteCallback(void)
{
    SPI1_WaitDone();
    ST7789_Unselect();
}





 
