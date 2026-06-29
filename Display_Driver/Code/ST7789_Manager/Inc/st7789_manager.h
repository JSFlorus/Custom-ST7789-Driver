#ifndef __st7789_manager_h
#define __st7789_manager_h
#include <stdint.h>
#include <stdbool.h>
/* ST7789 command registers */
#define ST7789_RAMWR      0x2C
#define ST7789_SWRESET    0x01
#define ST7789_NORON      0x13
#define ST7789_MADCTL     0x36
#define ST7789_COLMOD     0x3A
#define ST7789_INVON      0x21
#define ST7789_CASET      0x2A
#define ST7789_RASET      0x2B
#define ST7789_PORCTRL    0xB2
#define ST7789_GCTRL      0xB7
#define ST7789_VCOMS      0xBB
#define ST7789_LCMCTRL    0xC0
#define ST7789_VDVVRHEN   0xC2
#define ST7789_VRHS       0xC3
#define ST7789_VDVS       0xC4
#define ST7789_FRCTRL2    0xC6
#define ST7789_PWCTRL1    0xD0
#define ST7789_PVGAMCTRL  0xE0
#define ST7789_NVGAMCTRL  0xE1
#define ST7789_SLPOUT     0x11
#define ST7789_DISPON     0x29






typedef enum
{
    ST7789_FRAME_RATE_45HZ  = 1,
    ST7789_FRAME_RATE_60HZ  = 2,
    ST7789_FRAME_RATE_75HZ  = 3,
    ST7789_FRAME_RATE_90HZ  = 4,
    ST7789_FRAME_RATE_120HZ = 5
} ST7789_FrameRate;

typedef enum
{
    ST7789_ORIENTATION_0   = 1,
    ST7789_ORIENTATION_90  = 2,
    ST7789_ORIENTATION_180 = 3,
    ST7789_ORIENTATION_270 = 4
} ST7789_Orientation;

typedef enum
{
    ST7789_COLOR_RGB444 = 1,   // 12-bit, packed as 1.5 bytes/pixel
    ST7789_COLOR_RGB565 = 2,   // 16-bit, 2 bytes/pixel
    ST7789_COLOR_RGB666 = 3    // 18-bit, sent as 3 bytes/pixel over SPI
} ST7789_ColorMode;

typedef struct
{
    uint16_t height;
    uint16_t width;

    uint16_t x_offset;
    uint16_t y_offset;

    ST7789_ColorMode color_mode;
    ST7789_FrameRate frame_rate;
    ST7789_Orientation orientation;
} ST7789_Config;

extern ST7789_Config st7789;












bool ST7789_Update(void);
bool ST7789_UpdateWindow(uint16_t x,
                         uint16_t y,
                         uint16_t width,
                         uint16_t height);
bool ST7789_UpdateRows(uint16_t row, uint16_t width, uint32_t pixelCount);
bool ST7789_UpdateRowsDoubleBuffer(uint16_t row,
                                   uint16_t width,
                                   uint32_t pixelCount);
void ST7789_SetWindow(uint16_t column,
                      uint16_t row,
                      uint16_t height,
                      uint16_t width);
void ST7789_StartWrite(void);
void ST7789_Select(void);
void ST7789_Unselect(void);
void ST7789_SetDataMode(void);
void ST7789_Init(void);
void ST7789_Reset(void);
void ST7789_ApplyConfiguration(void);
void ST7789_WriteCommand(uint8_t cmd);
void ST7789_WriteData_Byte(uint8_t data);
void ST7789_SetOrientation(ST7789_Orientation orientation);
void ST7789_SetColorMode(ST7789_ColorMode colorMode);
void ST7789_DMACompleteCallback(void);
#endif
