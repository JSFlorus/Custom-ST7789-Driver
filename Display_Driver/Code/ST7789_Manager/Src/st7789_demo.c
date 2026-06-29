#include "st7789_demo.h"
#include "st7789_manager.h"
#include "dma_manager.h"
#include "spi1_dma2_manager.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

#define COLOR_RED    0xF800
#define COLOR_GREEN  0x07E0
#define COLOR_BLUE   0x001F
#define COLOR_BLACK  0x0000
#define COLOR_WHITE  0xFFFF
#define COLOR_YELLOW 0xFFE0
#define COLOR_ORANGE 0xFD20

#define DMA_DEMO_MAX_TRANSFERS 30000U

#define BOX_WIDTH       40U
#define BOX_HEIGHT      40U
#define BOX_STEP         4U
#define FRAME_DELAY_MS  20U

static bool DemoShowWindow(uint16_t x,
                           uint16_t y,
                           uint16_t width,
                           uint16_t height,
                           uint16_t color,
                           uint32_t delay_ms)
{
    uint32_t pixel_count = (uint32_t)width * height;

    if (width == 0U || height == 0U)
    {
        return false;
    }

    if ((uint32_t)x + width > st7789.width ||
        (uint32_t)y + height > st7789.height)
    {
        return false;
    }

    if (pixel_count > DMA_DEMO_MAX_TRANSFERS)
    {
        return false;
    }

    while (DMA_SPI1_IsBusy())
    {
    }

    if (!DMA_SPI1_SetTransferCount(pixel_count))
    {
        return false;
    }

    if (!DMA_SPI1_Write16ToBuffer(0, color))
    {
        return false;
    }

    if (!DMA_SPI1_Write16ToBuffer(1, color))
    {
        return false;
    }

    if (!ST7789_UpdateWindow(x, y, width, height))
    {
        return false;
    }

    while (DMA_SPI1_IsBusy())
    {
    }

    if (delay_ms > 0U)
    {
        HAL_Delay(delay_ms);
    }

    return true;
}

static bool DemoFillScreen(uint16_t color)
{
    uint16_t screen_width  = st7789.width;
    uint16_t screen_height = st7789.height;

    uint16_t strip_height =
        (uint16_t)(DMA_DEMO_MAX_TRANSFERS / screen_width);

    if (strip_height == 0U)
    {
        return false;
    }

    uint16_t y = 0U;

    while (y < screen_height)
    {
        uint16_t remaining_height = screen_height - y;
        uint16_t current_height = strip_height;

        if (current_height > remaining_height)
        {
            current_height = remaining_height;
        }

        if (!DemoShowWindow(0,
                    y,
                    screen_width,
                    current_height,
                    color,
                    0U))
        {
            return false;
        }

        y += current_height;
    }

    return true;
}
void ST7789_DemoShowWindowSequence(void)
{
    const uint16_t full_width  = st7789.width;
    const uint16_t full_height = st7789.height;

    const uint16_t half_width  = full_width / 2;
    const uint16_t half_height = full_height / 2;

    const uint16_t strip_height =
        (uint16_t)(DMA_DEMO_MAX_TRANSFERS / full_width);

    DemoShowWindow(0,          0,           half_width, half_height, COLOR_RED,   1000);
    DemoShowWindow(half_width, 0,           half_width, half_height, COLOR_GREEN, 1000);
    DemoShowWindow(0,          half_height, half_width, half_height, COLOR_BLUE,  1000);
    DemoShowWindow(half_width, half_height, half_width, half_height, COLOR_ORANGE, 1000);

    for (uint16_t y = 0; y < full_height; y += strip_height)
    {
        uint16_t remaining = full_height - y;
        uint16_t height = remaining;

        if (height > strip_height)
        {
            height = strip_height;
        }

        DemoShowWindow(0, y, full_width, height, COLOR_BLACK, 0);
    }

    DemoShowWindow((full_width - half_width) / 2,
                   (full_height - half_height) / 2,
                   half_width,
                   half_height,
                   COLOR_YELLOW,
                   10);
}


#define BOX_WIDTH       40U
#define BOX_HEIGHT      40U
#define BOX_STEP         8U
#define BOX_PADDING      BOX_STEP
#define FRAME_DELAY_MS  10U

#define MOVING_WINDOW_WIDTH \
    (BOX_WIDTH + (2U * BOX_PADDING))

#define MOVING_WINDOW_HEIGHT \
    (BOX_HEIGHT + (2U * BOX_PADDING))

#define MOVING_WINDOW_PIXELS \
    (MOVING_WINDOW_WIDTH * MOVING_WINDOW_HEIGHT)

static uint16_t moving_window[MOVING_WINDOW_PIXELS];
static void DemoCreateMovingWindow(void)
{
    const int32_t center_x =
        (2 * BOX_PADDING) + BOX_WIDTH - 1;

    const int32_t center_y =
        (2 * BOX_PADDING) + BOX_HEIGHT - 1;

    const int32_t diameter =
        (BOX_WIDTH < BOX_HEIGHT) ? BOX_WIDTH : BOX_HEIGHT;

    const int32_t radius_squared =
        diameter * diameter;

    for (uint16_t y = 0; y < MOVING_WINDOW_HEIGHT; y++)
    {
        for (uint16_t x = 0; x < MOVING_WINDOW_WIDTH; x++)
        {
            uint32_t index =
                ((uint32_t)y * MOVING_WINDOW_WIDTH) + x;

            /*
             * Coordinates are multiplied by two so an even-sized
             * circle can be centered between pixels.
             */
            int32_t dx = ((int32_t)x * 2) - center_x;
            int32_t dy = ((int32_t)y * 2) - center_y;

            if ((dx * dx) + (dy * dy) <= radius_squared)
            {
                moving_window[index] = COLOR_WHITE;
            }
            else
            {
                moving_window[index] = COLOR_BLACK;
            }
        }
    }
}
static bool DemoShowBufferWindow(uint16_t x,
                                 uint16_t y,
                                 uint16_t width,
                                 uint16_t height,
                                 const uint16_t *pixels,
                                 uint32_t delay_ms)
{
    uint32_t pixel_count = (uint32_t)width * height;

    if (pixels == NULL)
    {
        return false;
    }

    if (width == 0U || height == 0U)
    {
        return false;
    }

    if ((uint32_t)x + width > st7789.width ||
        (uint32_t)y + height > st7789.height)
    {
        return false;
    }

    if (pixel_count > DMA_DEMO_MAX_TRANSFERS)
    {
        return false;
    }

    while (DMA_SPI1_IsBusy())
    {
    }


    if (!DMA_SPI1_SetTransferCount(pixel_count))
    {
        return false;
    }

    if (!DMA_SPI1_WriteBufferToWritableBuffer(pixels))
    {
        return false;
    }

    if (!ST7789_UpdateWindow(x, y, width, height))
    {
        return false;
    }

    while (DMA_SPI1_IsBusy())
    {
    }

    if (delay_ms > 0U)
    {
        HAL_Delay(delay_ms);
    }

    return true;
}
void ST7789_DemoMovingBox(void)
{
    const int32_t maximum_x =
        (int32_t)st7789.width - MOVING_WINDOW_WIDTH;

    const int32_t maximum_y =
        (int32_t)st7789.height - MOVING_WINDOW_HEIGHT;

    if (maximum_x < 0 || maximum_y < 0)
    {
        return;
    }

    if (!DemoFillScreen(COLOR_BLACK))
    {
        return;
    }

    DemoCreateMovingWindow();

    int32_t window_x = 0;
    int32_t window_y = 0;

    int32_t x_direction = BOX_STEP;
    int32_t y_direction = BOX_STEP;

    while (1)
    {
        /*
         * Send the black border and white box together
         * in one DMA transfer.
         */
        if (!DemoShowBufferWindow(
                (uint16_t)window_x,
                (uint16_t)window_y,
                MOVING_WINDOW_WIDTH,
                MOVING_WINDOW_HEIGHT,
                moving_window,
                FRAME_DELAY_MS))
        {
            return;
        }

        window_x += x_direction;
        window_y += y_direction;

        if (window_x >= maximum_x)
        {
            window_x = maximum_x;
            x_direction = -BOX_STEP;
        }
        else if (window_x <= 0)
        {
            window_x = 0;
            x_direction = BOX_STEP;
        }

        if (window_y >= maximum_y)
        {
            window_y = maximum_y;
            y_direction = -BOX_STEP;
        }
        else if (window_y <= 0)
        {
            window_y = 0;
            y_direction = BOX_STEP;
        }
    }
}