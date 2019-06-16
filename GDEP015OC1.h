/* 
 * Copyright (c) 2019, Mahyar Koshkouei
 * Released under ISC License.
 * 
 * Library to operate the GDEP015OC1 display.
 */

#ifndef MK_GDEP015OC1_H
#define	MK_GDEP015OC1_H

/* User required definitions. */
#ifndef MK_GDEP_READ_BUSY_PIN
#error "MK_GDEP_READ_BUSY_PIN was not defined."
#endif

#ifndef MK_GDEP_RESET_DISPLAY
#error "MK_GDEP_SET_RESET_PIN was not defined."
#endif

#ifndef MK_GDEP_SET_DC_PIN
#error "MK_GDEP_SET_DC_PIN was not defined."
#endif

#ifndef MK_GDEP_SET_CS_PIN
#error "MK_GDEP_SET_CS_PIN was not defined."
#endif

#ifndef MK_GDEP_SET_SCLK_PIN
#error "MK_GDEP_SET_SCLK_PIN was not defined."
#endif

#ifndef MK_GDEP_SET_SDI_PIN
#error "MK_GDEP_SET_SDI_PIN was not defined."
#endif

#ifndef MK_GDEP_SEND_SPI_BYTE
#error "MK_GDEP_SEND_SPI_BYTE was not defined."
#endif

/* Display resolution */
#define GDEP015OC1_DISPLAY_WIDTH		200
#define GDEP015OC1_DISPLAY_HEIGHT		200

/* GDEP015OC1 commands */
#define DRIVER_OUTPUT_CONTROL			0x01
#define BOOSTER_SOFT_START_CONTROL		0x0C
#define GATE_SCAN_START_POSITION		0x0F
#define DEEP_SLEEP_MODE				0x10
#define DATA_ENTRY_MODE_SETTING			0x11
#define SW_RESET				0x12
#define TEMPERATURE_SENSOR_CONTROL		0x1A
#define MASTER_ACTIVATION			0x20
#define DISPLAY_UPDATE_CONTROL_1		0x21
#define DISPLAY_UPDATE_CONTROL_2		0x22
#define WRITE_RAM				0x24
#define WRITE_VCOM_REGISTER			0x2C
#define WRITE_LUT_REGISTER			0x32
#define SET_DUMMY_LINE_PERIOD			0x3A
#define SET_GATE_TIME				0x3B
#define BORDER_WAVEFORM_CONTROL			0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION	0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION	0x45
#define SET_RAM_X_ADDRESS_COUNTER		0x4E
#define SET_RAM_Y_ADDRESS_COUNTER		0x4F
#define TERMINATE_FRAME_READ_WRITE		0xFF

enum lut_selection_e {
	LUT_NONE,	/* Uses previously used LUT? */
	LUT_FULL,
	LUT_PARTIAL
};

const uint8_t lut_full_update[] =
{
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 
    0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51, 
    0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

const uint8_t lut_partial_update[] =
{
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/**
 * Transmits a given buffer over SPI.
 * 
 * @param buf buffer to transmit
 * @param len length of buffer
 */
void send_spi_buffer(const uint8_t *buf, const uint_fast8_t len)
{
	// Select data transmission
	MK_GDEP_SET_DC_PIN(1);

	// Select display for SPI communication
	MK_GDEP_SET_CS_PIN(0);

	for(uint_fast8_t i = 0; i < len; i++)
		MK_GDEP_SEND_SPI_BYTE(buf[i]);

	MK_GDEP_SET_CS_PIN(1);
}

/**
 * Send command over SPI to the display.
 * 
 * @param cmd Command to send.
 */
void send_disp_command(const uint8_t cmd)
{
	// Select command transmission
	MK_GDEP_SET_DC_PIN(0);

	// Select display for SPI communication
	MK_GDEP_SET_CS_PIN(0);

	MK_GDEP_SEND_SPI_BYTE(cmd);

	MK_GDEP_SET_CS_PIN(1);
}

/**
 * Waits until the display is idle.
 */
inline void disp_wait_until_idle(void)
{
	while(MK_GDEP_READ_BUSY_PIN);
}

/**
 * Initialise the connected display.
 */
inline void init_display(const enum lut_selection_e lut)
{
	MK_GDEP_RESET_DISPLAY();

	send_disp_command(DRIVER_OUTPUT_CONTROL);
	const uint8_t doc_data[] = {
		0xC7, // Low 8 bits for display height
		0x00, // High 8 bits
		0x00
		//	0b00000000
		//	  UUUUU
		//	       0   GD: Selects gate layout of panel
		//	        0  SM: Disable interleaved mode
		//	         0 TB: Scanning direction of gate driver
	};
	send_spi_buffer(doc_data, sizeof (doc_data));

	send_disp_command(BOOSTER_SOFT_START_CONTROL);
	const uint8_t bssc_data[] = {0xD7, 0xD6, 0x9D};
	send_spi_buffer(bssc_data, sizeof (bssc_data));

	send_disp_command(WRITE_VCOM_REGISTER);
	const uint8_t wvr_data[] = {0xA8};
	send_spi_buffer(wvr_data, sizeof (wvr_data));

	send_disp_command(SET_DUMMY_LINE_PERIOD);
	const uint8_t sdlp_data[] = {0x1A};
	send_spi_buffer(sdlp_data, sizeof (sdlp_data));

	send_disp_command(SET_GATE_TIME);
	const uint8_t sgt_data[] = {0x08};
	send_spi_buffer(sgt_data, sizeof (sgt_data));

	send_disp_command(DATA_ENTRY_MODE_SETTING);
	const uint8_t dems_data[] = {0x03};
	send_spi_buffer(dems_data, sizeof (dems_data));

	switch(lut)
	{
	case LUT_FULL:
		send_disp_command(WRITE_LUT_REGISTER);
		send_spi_buffer(lut_full_update, sizeof (lut_full_update));
		break;
		
	case LUT_PARTIAL:
		send_disp_command(WRITE_LUT_REGISTER);
		send_spi_buffer(lut_partial_update, sizeof(lut_partial_update));
		break;
		
	default:
		break;
	};
}

void disp_set_memory_area(const uint8_t x_start, const uint8_t y_start,
			  const uint8_t x_end, const uint8_t y_end)
{
	send_disp_command(SET_RAM_X_ADDRESS_START_END_POSITION);
	/* x point must be the multiple of 8 or the last 3 bits will be ignored */
	uint8_t set_ramx_addr[] = {(x_start >> 3), (x_end >> 3)};
	send_spi_buffer(set_ramx_addr, sizeof (set_ramx_addr));

	send_disp_command(SET_RAM_Y_ADDRESS_START_END_POSITION);
	uint8_t set_ramy_addr[] = {y_start, 0x00, y_end, 0x00};
	send_spi_buffer(set_ramy_addr, sizeof (set_ramy_addr));
}

void set_memory_pointer(uint8_t x, uint8_t y)
{
	send_disp_command(SET_RAM_X_ADDRESS_COUNTER);
	/* x point must be the multiple of 8 or the last 3 bits will be ignored */
	uint8_t set_ramx_addr_count[] = {x >> 3};
	send_spi_buffer(set_ramx_addr_count, sizeof (set_ramx_addr_count));

	send_disp_command(SET_RAM_Y_ADDRESS_COUNTER);
	uint8_t set_ramy_addr_count[] = {y, 0x00};
	send_spi_buffer(set_ramy_addr_count, sizeof (set_ramy_addr_count));

	disp_wait_until_idle();
}

void disp_clear_frame_memory(const uint8_t color)
{
	disp_set_memory_area(0, 0,
			GDEP015OC1_DISPLAY_WIDTH - 1, GDEP015OC1_DISPLAY_HEIGHT - 1);
	set_memory_pointer(0, 0);
	send_disp_command(WRITE_RAM);

	// Select data transmission
	MK_GDEP_SET_DC_PIN(1);

	// Select display for SPI communication
	MK_GDEP_SET_CS_PIN(0);

	/* send the colour data */
	uint_fast16_t i = (GDEP015OC1_DISPLAY_WIDTH / 8) * GDEP015OC1_DISPLAY_HEIGHT;
	do
	{
		MK_GDEP_SEND_SPI_BYTE(color);
	}
	while(--i);

	MK_GDEP_SET_CS_PIN(1);
}

void disp_display_frame(void)
{
	send_disp_command(DISPLAY_UPDATE_CONTROL_2);
	const uint8_t duc2_data[] = {0xC4};
	send_spi_buffer(duc2_data, sizeof (duc2_data));

	send_disp_command(MASTER_ACTIVATION);
	send_disp_command(TERMINATE_FRAME_READ_WRITE);

	disp_wait_until_idle();
}

void disp_sleep(void)
{
	send_disp_command(DEEP_SLEEP_MODE);

	const uint8_t sleep_data[] = {0x01};
	send_spi_buffer(sleep_data, sizeof (sleep_data));
}

void disp_set_block(const uint8_t x, const uint8_t y,
		    const uint8_t x_end, const uint8_t y_end,
		    const uint8_t pattern)
{

	uint16_t pixels = (((x_end + 1) - x) * ((y_end + 1) - y));

	disp_set_memory_area(x, y, x_end, y_end);
	set_memory_pointer(x, y);
	send_disp_command(WRITE_RAM);

	// Select data transmission
	MK_GDEP_SET_DC_PIN(1);

	// Select display for SPI communication
	MK_GDEP_SET_CS_PIN(0);

	/* send the pattern data */
	do
	{
		MK_GDEP_SEND_SPI_BYTE(pattern);
	}
	while(--pixels);

	MK_GDEP_SET_CS_PIN(1);
}

void disp_write_image(const uint8_t x, const uint8_t y,
		      const uint8_t x_end, const uint8_t y_end,
		      const uint8_t *img, const uint_fast16_t len)
{
	disp_set_memory_area(x, y, x_end, y_end);
	set_memory_pointer(x, y);
	send_disp_command(WRITE_RAM);

	// Select data transmission
	MK_GDEP_SET_DC_PIN(1);

	// Select display for SPI communication
	MK_GDEP_SET_CS_PIN(0);

	/* send the pattern data */
	for(uint_fast16_t i = 0; i < len; i++)
		MK_GDEP_SEND_SPI_BYTE(img[i]);

	MK_GDEP_SET_CS_PIN(1);
}

#endif	/* MK_GDEP015OC1_H */
