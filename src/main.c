#include <stdio.h>
#include <time.h>

#include "common/platform.h"
#include "common/cs_file.h"
#include "fw/src/mgos_app.h"
#include "fw/src/mgos_gpio.h"
#include "fw/src/mgos_sys_config.h"
#include "fw/src/mgos_timers.h"
#include "fw/src/mgos_hal.h"
#include "fw/src/mgos_dlsym.h"


#include "mgos.h"

#include "tftspi.h"
#include "tft.h"

// #include "spiffs_vfs.h"


// ==================================================
// Define which spi bus to use VSPI_HOST or HSPI_HOST
#define SPI_BUS HSPI_HOST

void tft_init(void)
{

    // ========  PREPARE DISPLAY INITIALIZATION  =========

    esp_err_t ret;

    // === SET GLOBAL VARIABLES ==========================

    // ===================================================
    // ==== Set display type                         =====
    tft_disp_type = DEFAULT_DISP_TYPE;
	//tft_disp_type = DISP_TYPE_ILI9341;
	//tft_disp_type = DISP_TYPE_ILI9488;
	//tft_disp_type = DISP_TYPE_ST7735B;
    // ===================================================

	// ===================================================
	// === Set display resolution if NOT using default ===
	// === DEFAULT_TFT_DISPLAY_WIDTH &                 ===
    // === DEFAULT_TFT_DISPLAY_HEIGHT                  ===
	_width = DEFAULT_TFT_DISPLAY_WIDTH;  // smaller dimension
	_height = DEFAULT_TFT_DISPLAY_HEIGHT; // larger dimension
	//_width = 128;  // smaller dimension
	//_height = 160; // larger dimension
	// ===================================================

	// ===================================================
	// ==== Set maximum spi clock for display read    ====
	//      operations, function 'find_rd_speed()'    ====
	//      can be used after display initialization  ====
	max_rdclock = 8000000;
	// ===================================================

    // ====================================================================
    // === Pins MUST be initialized before SPI interface initialization ===
    // ====================================================================
    TFT_PinsInit();

    // ====  CONFIGURE SPI DEVICES(s)  ====================================================================================

    spi_lobo_device_handle_t spi;
	
    spi_lobo_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,				// set SPI MISO pin
        .mosi_io_num=PIN_NUM_MOSI,				// set SPI MOSI pin
        .sclk_io_num=PIN_NUM_CLK,				// set SPI CLK pin
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
		.max_transfer_sz = 6*1024,
    };
    spi_lobo_device_interface_config_t devcfg={
        .clock_speed_hz=8000000,                // Initial clock out at 8 MHz
        .mode=0,                                // SPI mode 0
        .spics_io_num=-1,                       // we will use external CS pin
		.spics_ext_io_num=PIN_NUM_CS,           // external CS pin
		.flags=SPI_DEVICE_HALFDUPLEX,           // ALWAYS SET  to HALF DUPLEX MODE!! for display spi
    };

#if USE_TOUCH == TOUCH_TYPE_XPT2046
    spi_lobo_device_handle_t tsspi = NULL;

    spi_lobo_device_interface_config_t tsdevcfg={
        .clock_speed_hz=2500000,                //Clock out at 2.5 MHz
        .mode=0,                                //SPI mode 0
        .spics_io_num=PIN_NUM_TCS,              //Touch CS pin
		.spics_ext_io_num=-1,                   //Not using the external CS
		//.command_bits=8,                        //1 byte command
    };
#elif USE_TOUCH == TOUCH_TYPE_STMPE610
    spi_lobo_device_handle_t tsspi = NULL;

    spi_lobo_device_interface_config_t tsdevcfg={
        .clock_speed_hz=1000000,                //Clock out at 1 MHz
        .mode=STMPE610_SPI_MODE,                //SPI mode 0
        .spics_io_num=PIN_NUM_TCS,              //Touch CS pin
		.spics_ext_io_num=-1,                   //Not using the external CS
        .flags = 0,
    };
#endif

    // ====================================================================================================================


    vTaskDelay(500 / portTICK_RATE_MS);
	printf("\r\n==============================\r\n");
    printf("TFT display DEMO, LoBo 10/2017\r\n");
	printf("==============================\r\n");
    printf("Pins used: miso=%d, mosi=%d, sck=%d, cs=%d\r\n", PIN_NUM_MISO, PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_CS);
#if USE_TOUCH > TOUCH_TYPE_NONE
    printf(" Touch CS: %d\r\n", PIN_NUM_TCS);
#endif
	printf("==============================\r\n\r\n");

	// ==================================================================
	// ==== Initialize the SPI bus and attach the LCD to the SPI bus ====

	ret=spi_lobo_bus_add_device(SPI_BUS, &buscfg, &devcfg, &spi);
    assert(ret==ESP_OK);
	printf("SPI: display device added to spi bus (%d)\r\n", SPI_BUS);
	disp_spi = spi;

	// ==== Test select/deselect ====
	ret = spi_lobo_device_select(spi, 1);
    assert(ret==ESP_OK);
	ret = spi_lobo_device_deselect(spi);
    assert(ret==ESP_OK);

	printf("SPI: attached display device, speed=%u\r\n", spi_lobo_get_speed(spi));
	printf("SPI: bus uses native pins: %s\r\n", spi_lobo_uses_native_pins(spi) ? "true" : "false");

#if USE_TOUCH > TOUCH_TYPE_NONE
	// =====================================================
    // ==== Attach the touch screen to the same SPI bus ====

	ret=spi_lobo_bus_add_device(SPI_BUS, &buscfg, &tsdevcfg, &tsspi);
    assert(ret==ESP_OK);
	printf("SPI: touch screen device added to spi bus (%d)\r\n", SPI_BUS);
	ts_spi = tsspi;

	// ==== Test select/deselect ====
	ret = spi_lobo_device_select(tsspi, 1);
    assert(ret==ESP_OK);
	ret = spi_lobo_device_deselect(tsspi);
    assert(ret==ESP_OK);

	printf("SPI: attached TS device, speed=%u\r\n", spi_lobo_get_speed(tsspi));
#endif

	// ================================
	// ==== Initialize the Display ====

	printf("SPI: display init...\r\n");
	TFT_display_init();
    printf("OK\r\n");
    #if USE_TOUCH == TOUCH_TYPE_STMPE610
	stmpe610_Init();
	vTaskDelay(10 / portTICK_RATE_MS);
    uint32_t tver = stmpe610_getID();
    printf("STMPE touch initialized, ver: %04x - %02x\r\n", tver >> 8, tver & 0xFF);
    #endif
	
	// ---- Detect maximum read speed ----
	max_rdclock = find_rd_speed();
	printf("SPI: Max rd speed = %u\r\n", max_rdclock);

    // ==== Set SPI clock used for display operations ====
	spi_lobo_set_speed(spi, DEFAULT_SPI_CLOCK);
	printf("SPI: Changed speed to %u\r\n", spi_lobo_get_speed(spi));

    printf("\r\n---------------------\r\n");
	printf("Graphics demo started\r\n");
	printf("---------------------\r\n");

	font_rotate = 0;
	text_wrap = 0;
	font_transparent = 0;
	font_forceFixed = 0;
	gray_scale = 0;
    TFT_setGammaCurve(DEFAULT_GAMMA_CURVE);

    TFT_jpg_image(CENTER, CENTER, 2, "/images/test2.jpg", NULL, 0);


}

//
//
enum mgos_app_init_result mgos_app_init(void) {

//	tft_init();

 	return MGOS_APP_INIT_SUCCESS;
}
