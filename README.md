# STM32 Display Driver

This project is an STM32F407 display-driver experiment for driving an ST7789 TFT display over SPI using DMA double-buffer mode.

## Custom Drivers

The code is organized around four project-specific driver/demo modules:

### SPI1 DMA2 Manager

Location:

- `Display_Driver/Code/SPI1_DMA2_Manager`

This layer owns the low-level SPI1 register helpers used by the display path.

Main responsibilities:

- Enable and disable SPI1.
- Switch SPI1 between 8-bit command mode and 16-bit RGB565 pixel mode.
- Write 8-bit ST7789 commands/data.
- Wait for SPI transfer completion before releasing display chip select.

### DMA Manager

Location:

- `Display_Driver/Code/DMA_Manager`

This layer manages DMA2 Stream3 for SPI1 TX transfers.

Main responsibilities:

- Configure DMA2 Stream3 for memory-to-peripheral transfers.
- Use double-buffer mode with `M0AR` and `M1AR`.
- Track the active/writable target buffer using the DMA hardware `CT` bit.
- Fill DMA buffers with RGB565 pixel data.
- Start DMA transfers into `SPI1->DR`.
- Handle DMA transfer-complete interrupts and notify the display driver.

### ST7789 Manager

Location:

- `Display_Driver/Code/ST7789_Manager`

This layer owns the ST7789 display protocol.

Main responsibilities:

- Reset and initialize the ST7789 controller.
- Configure color mode, orientation, and frame rate.
- Set the active display window using `CASET` and `RASET`.
- Start RAM writes with `RAMWR`.
- Control display pins such as `CS`, `DC`, and `RST`.
- Coordinate DMA completion cleanup by waiting for SPI idle and releasing chip select.

The active display state is held in:

```c
ST7789_Config st7789;
```

The demo and display update code use this config for width, height, and offsets.

### ST7789 Demo

Location:

- `Display_Driver/Code/ST7789_Manager/Src/st7789_demo.c`
- `Display_Driver/Code/ST7789_Manager/Inc/st7789_demo.h`

This module contains the display test/demo sequence.

Main responsibilities:

- Draw colored regions on the display.
- Exercise windowed ST7789 updates.
- Exercise the DMA double-buffer transfer path.
- Demonstrate corner fills, screen clearing, and moving/center box behavior.

The application calls:

```c
ST7789_DemoShowWindowSequence();
```

from the main loop.

## Runtime Flow

At startup, `main.c` initializes the MCU and display stack:

1. `HAL_Init()`
2. `SystemClock_Config()`
3. `MX_GPIO_Init()`
4. `MX_SPI1_Init()`
5. `Init_SPI1_DMA2()`
6. `NVIC_EnableIRQ(DMA2_Stream3_IRQn)`
7. `ST7789_Init()`

Then the main loop runs the ST7789 demo.

## Data Flow

The pixel-transfer path is:

```text
Demo
  -> ST7789 Manager sets display window
  -> DMA Manager fills double buffers
  -> DMA2 Stream3 writes RGB565 data to SPI1->DR
  -> SPI1 sends pixels to ST7789 over MOSI/SCK
  -> DMA interrupt completes transfer
  -> ST7789 callback waits for SPI idle and releases CS
```

## Hardware Path

```text
STM32F407
  SPI1 SCK/MOSI  -> ST7789 SPI input
  GPIO CS/DC/RST -> ST7789 control pins
  DMA2 Stream3   -> SPI1 TX data register
```

SPI1 is configured for a 42 MHz SCK with the current clock tree and `SPI_BAUDRATEPRESCALER_2`.

