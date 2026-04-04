/*
 * Board configuration for: Waveshare ESP32-S3-Touch-LCD-2.8B
 *   Resolution:  480 × 480
 *   LCD:         RGB parallel + 3-wire SPI control panel (GC9503 or ST7701)
 *   Touch:       CST816S / GT911 via I2C (GPIO15=SDA, GPIO7=SCL)
 *   IO Expander: provides LCD_RST (EXIO1), TP_RST (EXIO2), LCD_CS (EXIO3)
 *   Backlight:   GPIO6 (PWM via LEDC)
 *
 * USAGE:
 *   OPTION A (recommended): Let the ESP32_Display_Panel library use its
 *     predefined board config. This is selected by the build_flags entry
 *     `-DESP_PANEL_BOARD_WAVESHARE_ESP32_S3_TOUCH_LCD_2_8_B` in platformio.ini.
 *     Keep ESP_PANEL_BOARD_DEFAULT_USE_CUSTOM = 0 below.
 *
 *   OPTION B (fallback): If the library does NOT recognise the board define,
 *     set ESP_PANEL_BOARD_DEFAULT_USE_CUSTOM = 1 and uncomment / complete the
 *     manual configuration below.  You will also need to know the exact LCD
 *     driver IC (GC9503? ST7701?) and IO expander chip.
 */
#pragma once

// *INDENT-OFF*

// File version — must match ESP_PANEL_BOARD_CUSTOM_VERSION_* in esp_panel_versions.h
#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_MAJOR 1
#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_MINOR 2
#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_PATCH 0

// 0 = use predefined board from library (see platformio.ini build_flags)
// 1 = use the custom config defined in this file
#define ESP_PANEL_BOARD_DEFAULT_USE_CUSTOM  (0)

#if ESP_PANEL_BOARD_DEFAULT_USE_CUSTOM
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OPTION B – manual config (uncomment and fill in once you know your exact hardware)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ESP_PANEL_BOARD_NAME    "Waveshare:ESP32-S3-Touch-LCD-2.8B"
#define ESP_PANEL_BOARD_WIDTH   (480)
#define ESP_PANEL_BOARD_HEIGHT  (480)

// ── LCD (RGB 16-bit + 3-wire SPI control) ───────────────────────────────────
#define ESP_PANEL_BOARD_USE_LCD     (1)
#if ESP_PANEL_BOARD_USE_LCD

// Replace with actual controller: GC9503 | ST7701 | etc.
#define ESP_PANEL_BOARD_LCD_CONTROLLER      GC9503
#define ESP_PANEL_BOARD_LCD_BUS_TYPE        (ESP_PANEL_BUS_TYPE_RGB)
#define ESP_PANEL_BOARD_LCD_RGB_USE_CONTROL_PANEL   (1) // 3-wire SPI enabled

#if ESP_PANEL_BOARD_LCD_RGB_USE_CONTROL_PANEL
    // 3-wire SPI (control panel)
    #define ESP_PANEL_BOARD_LCD_RGB_SPI_IO_CS           (3)  // EXIO3 (IO expander port 3)
    #define ESP_PANEL_BOARD_LCD_RGB_SPI_IO_SCK          (2)  // GPIO2 = LCD_SCL
    #define ESP_PANEL_BOARD_LCD_RGB_SPI_IO_SDA          (1)  // GPIO1 = LCD_SDA
    #define ESP_PANEL_BOARD_LCD_RGB_SPI_CS_USE_EXPNADER (1)  // CS is on IO expander
    #define ESP_PANEL_BOARD_LCD_RGB_SPI_SCL_USE_EXPNADER (0)
    #define ESP_PANEL_BOARD_LCD_RGB_SPI_SDA_USE_EXPNADER (0)
    #define ESP_PANEL_BOARD_LCD_RGB_SPI_MODE            (0)
    #define ESP_PANEL_BOARD_LCD_RGB_SPI_CMD_BYTES       (1)
    #define ESP_PANEL_BOARD_LCD_RGB_SPI_PARAM_BYTES     (1)
    #define ESP_PANEL_BOARD_LCD_RGB_SPI_USE_DC_BIT      (1)
#endif

// RGB panel timing – fill in from LCD datasheet
#define ESP_PANEL_BOARD_LCD_RGB_CLK_HZ          (16 * 1000 * 1000)
#define ESP_PANEL_BOARD_LCD_RGB_HPW             (10)
#define ESP_PANEL_BOARD_LCD_RGB_HBP             (10)
#define ESP_PANEL_BOARD_LCD_RGB_HFP             (20)
#define ESP_PANEL_BOARD_LCD_RGB_VPW             (10)
#define ESP_PANEL_BOARD_LCD_RGB_VBP             (10)
#define ESP_PANEL_BOARD_LCD_RGB_VFP             (10)
#define ESP_PANEL_BOARD_LCD_RGB_PCLK_ACTIVE_NEG (0)
#define ESP_PANEL_BOARD_LCD_RGB_DATA_WIDTH      (16)
#define ESP_PANEL_BOARD_LCD_RGB_PIXEL_BITS      (ESP_PANEL_LCD_COLOR_BITS_RGB565)
#define ESP_PANEL_BOARD_LCD_RGB_BOUNCE_BUF_SIZE (480 * 10)
#define ESP_PANEL_BOARD_LCD_RGB_IO_HSYNC        (38)   // HSYNC = GPIO38
#define ESP_PANEL_BOARD_LCD_RGB_IO_VSYNC        (39)   // VSYNC = GPIO39
#define ESP_PANEL_BOARD_LCD_RGB_IO_DE           (40)   // DE    = GPIO40
#define ESP_PANEL_BOARD_LCD_RGB_IO_PCLK         (41)   // PCLK  = GPIO41
#define ESP_PANEL_BOARD_LCD_RGB_IO_DISP         (-1)

// RGB data pins: DATA0 = B[0] (maps to B1=GPIO5 since B0=NC)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA0        (5)    // B1
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA1        (45)   // B2
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA2        (48)   // B3
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA3        (47)   // B4
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA4        (21)   // B5
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA5        (14)   // G0
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA6        (13)   // G1
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA7        (12)   // G2
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA8        (11)   // G3
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA9        (10)   // G4
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA10       (9)    // G5
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA11       (46)   // R1 (R0=NC)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA12       (3)    // R2
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA13       (8)    // R3
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA14       (18)   // R4
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA15       (17)   // R5

// Enable IO multiplex (3-wire SPI pins shared with RGB data → released after init)
#define ESP_PANEL_BOARD_LCD_FLAGS_ENABLE_IO_MULTIPLEX   (0)
#define ESP_PANEL_BOARD_LCD_FLAGS_MIRROR_BY_CMD         (1)

// Color config
#define ESP_PANEL_BOARD_LCD_COLOR_BITS          (ESP_PANEL_LCD_COLOR_BITS_RGB565)
#define ESP_PANEL_BOARD_LCD_COLOR_BGR_ORDER     (0)
#define ESP_PANEL_BOARD_LCD_COLOR_INEVRT_BIT    (0)
#define ESP_PANEL_BOARD_LCD_SWAP_XY             (0)
#define ESP_PANEL_BOARD_LCD_MIRROR_X            (0)
#define ESP_PANEL_BOARD_LCD_MIRROR_Y            (0)
#define ESP_PANEL_BOARD_LCD_GAP_X               (0)
#define ESP_PANEL_BOARD_LCD_GAP_Y               (0)

// LCD RST is on IO expander EXIO1 – handled by expander driver, not directly here
#define ESP_PANEL_BOARD_LCD_RST_IO              (-1)
#define ESP_PANEL_BOARD_LCD_RST_LEVEL           (0)
#endif // ESP_PANEL_BOARD_USE_LCD

// ── Touch (I2C) ──────────────────────────────────────────────────────────────
#define ESP_PANEL_BOARD_USE_TOUCH   (1)
#if ESP_PANEL_BOARD_USE_TOUCH
// Replace CST816S with GT911 if that is what's on your board
#define ESP_PANEL_BOARD_TOUCH_CONTROLLER        CST816S
#define ESP_PANEL_BOARD_TOUCH_BUS_TYPE          (ESP_PANEL_BUS_TYPE_I2C)
#define ESP_PANEL_BOARD_TOUCH_BUS_SKIP_INIT_HOST (0)
#define ESP_PANEL_BOARD_TOUCH_I2C_HOST_ID       (0)
#define ESP_PANEL_BOARD_TOUCH_I2C_CLK_HZ        (400 * 1000)
#define ESP_PANEL_BOARD_TOUCH_I2C_SCL_PULLUP    (1)
#define ESP_PANEL_BOARD_TOUCH_I2C_SDA_PULLUP    (1)
#define ESP_PANEL_BOARD_TOUCH_I2C_IO_SCL        (7)    // TP_SCL = GPIO7
#define ESP_PANEL_BOARD_TOUCH_I2C_IO_SDA        (15)   // TP_SDA = GPIO15
#define ESP_PANEL_BOARD_TOUCH_I2C_ADDRESS       (0)    // 0 = use driver default
#define ESP_PANEL_BOARD_TOUCH_SWAP_XY           (0)
#define ESP_PANEL_BOARD_TOUCH_MIRROR_X          (0)
#define ESP_PANEL_BOARD_TOUCH_MIRROR_Y          (0)
// TP_RST is on IO expander EXIO2 – managed by the library's board driver
#define ESP_PANEL_BOARD_TOUCH_RST_IO            (-1)
#define ESP_PANEL_BOARD_TOUCH_RST_LEVEL         (0)
#define ESP_PANEL_BOARD_TOUCH_INT_IO            (16)   // TP_INT = GPIO16
#define ESP_PANEL_BOARD_TOUCH_INT_LEVEL         (0)
#endif // ESP_PANEL_BOARD_USE_TOUCH

// ── Backlight (PWM via LEDC) ─────────────────────────────────────────────────
#define ESP_PANEL_BOARD_USE_BACKLIGHT   (1)
#if ESP_PANEL_BOARD_USE_BACKLIGHT
#define ESP_PANEL_BOARD_BACKLIGHT_TYPE          (ESP_PANEL_BACKLIGHT_TYPE_PWM_LEDC)
#define ESP_PANEL_BOARD_BACKLIGHT_IO            (6)    // LCD_BL = GPIO6
#define ESP_PANEL_BOARD_BACKLIGHT_ON_LEVEL      (1)
#define ESP_PANEL_BOARD_BACKLIGHT_IDLE_OFF      (0)
#endif

#endif // ESP_PANEL_BOARD_DEFAULT_USE_CUSTOM

// *INDENT-ON*
