/**
 * Copyright (c) 2019, Mahyar Koshkouei
 * Released under ISC License.
 * 
 * Displays Hello World image onto the MH-ET Live 1.54 E-paper Module, which
 * uses the GDEP015OC1 screen by Good Display, which uses the IL3829 display
 * driver.
 */

#define _XTAL_FREQ 1000000

#define MK_LIB_READ_BUSY_PIN		PORTBbits.RB0
#define MK_LIB_SET_RESET_PIN(x)		PORTBbits.RB1 = x
#define MK_LIB_SET_DC_PIN(x)		PORTBbits.RB2 = x
#define MK_LIB_SET_CS_PIN(x)		PORTBbits.RB3 = x
#define MK_LIB_SET_SCLK_PIN(x)		PORTBbits.RB4 = x
#define MK_LIB_SET_SDI_PIN(x)		PORTBbits.RB5 = x
#define MK_LIB_SEND_SPI_BYTE(x)		send_spi_byte(x)

#include <stdint.h>
#include <xc.h>

#include "GDEP015OC1.h"
#include "pic_config_bits.h"

/* Bitmaps to draw. */
#include "w.h"
#include "hello_world.h"

inline void send_spi_byte(const uint8_t byte)
{
	uint_fast8_t unused_read;
	SSP1BUF = byte;
	while (!SSP1STATbits.BF);
	unused_read = SSP1BUF;
}

/**
 * Transmits a given buffer over SPI.
 * 
 * @param buf buffer to transmit
 * @param len length of buffer
 */
void send_spi_buffer(const uint8_t *buf, const uint_fast8_t len)
{
	// Select data transmission
	MK_LIB_SET_DC_PIN(1);

	// Select display for SPI communication
	MK_LIB_SET_CS_PIN(0);

	for(uint_fast8_t i = 0; i < len; i++)
		MK_LIB_SEND_SPI_BYTE(buf[i]);

	MK_LIB_SET_CS_PIN(1);
}

/**
 * Send command over SPI to the display.
 * 
 * @param cmd Command to send.
 */
void send_disp_command(const uint8_t cmd)
{
	// Select command transmission
	MK_LIB_SET_DC_PIN(0);

	// Select display for SPI communication
	MK_LIB_SET_CS_PIN(0);

	MK_LIB_SEND_SPI_BYTE(cmd);

	MK_LIB_SET_CS_PIN(1);
}

/**
 * Waits until the display is idle.
 */
inline void disp_wait_until_idle(void)
{
	while(MK_LIB_READ_BUSY_PIN == 1);
}

/**
 * Initialise PIC peripherals.
 */
inline void init_pic(void)
{
	/* Disable unused peripherals. */
	PMD0 = 0b01111111;
	//         U
	//       0        SYSCMD: Enable Peripheral System Clock Network
	//        1       FVRMD: Disable Fixed Voltage Reference (FVR)
	//          1     CRCMD: Disable CRC module
	//           1    SCANMD: Disable Program Memory Scanner Module
	//            1   NVMMD: Disable NVM Module
	//             1  CLKRMD: Disable Clock Reference CLKR
	//              1 IOCMD: Disable Interrupt-on-Change bit, All Ports
	
	PMD1 = 0b11111111;
	//       1        NCOMD: Disable Numerically Control Oscillator 
	//        1111111 TMRxMD: Disable all timers
	
	PMD2 = 0b11111111;
	//       U  UU
	//        1       DACMD: Disable DAC
	//         1      ADCMD: Disable ADC
	//            11  CMPxMD: Disable all comparators
	//              1 ZCDMD: Disable ZCD
	
	PMD3 = 0b11111111;
	//       U
	//        11      PWMxMD: Disable all Pulse-Width Modulators
	//          11111 CCPxMD: Disable all Comparators
	
	PMD4 = 0b11101111;
	//       U   U
	//        1       UART1MD: Disable EUSART
	//         10     MSSPxMD: Enable MSSP1MD only.
	//            111 CWG1MD: Disable CWG
	
	PMD5 = 0b11111111;
	//         U
	//       11       SMTxMD: Disable Signal Measurement Timers
	//          1111  CLCxMD: Disable all CLCs
	//              1 DSMMD: Disable Data Signal Modulator

	/* Initialise IO Ports */
	PORTA = 0x00; // Initialise PORTA pins to low
	PORTB = 0x00;
	PORTC = 0x00;
	LATA = 0x00; // Initialise PORTA latches
	LATB = 0x00;
	LATC = 0x00;
	ANSELA = 0x00; // Set PORTA pins to digital
	ANSELB = 0x00;
	ANSELC = 0x00;
	TRISA = 0x00; // Set PORTA pins to input(1)/output(0)
	//        XX       Unused
	TRISB = 0b00000001;
	//        XX       Unused
	//          0      TRISB5: RB5 output to IL3829 SDI pin
	//           0     TRISB4: RB4 output to IL3829 SCLK pin
	//            0    TRISB3: RB3 output to IL3829 CS pin
	//             0   TRISB2: RB2 output to IL3829 DC pin
	//              0  TRISB1: RB1 output to IL3829 RESET pin
	//               1 TRISB0: RB0 input from IL3829 BUSY pin
	TRISC = 0x00;
	//        XX       Unused
	
	/* Set sleep mode to lowest power. */
	VREGCONbits.VREGPM = 1;

	/* SPI settings */
	SSP1CON1 = 0b00100000;
	//           NN N
	//             1       Enable SPI
	//               0000  SPI Master Clock @ FOSC/4

	/* IL3829 requires SPI sample on clock high. */
	SSP1STATbits.CKE = 1;

	RB5PPS = 0x15; // Set RB5 to SDO1
	RB4PPS = 0x14; // Set RB4 to SCLK1

	MK_LIB_SET_CS_PIN(1);
}

/**
 * Initialise the connected display.
 */
inline void init_display(void)
{
	// TODO: Check what the busy pin is doing here.
	MK_LIB_SET_RESET_PIN(0);
	NOP();
	MK_LIB_SET_RESET_PIN(1);
	NOP();

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

	send_disp_command(WRITE_LUT_REGISTER);
	//send_spi_buffer(lut_full_update, sizeof(lut_full_update));
	send_spi_buffer(lut_partial_update, sizeof(lut_partial_update));
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
			 IL3829_DISPLAY_WIDTH - 1, IL3829_DISPLAY_HEIGHT - 1);
	set_memory_pointer(0, 0);
	send_disp_command(WRITE_RAM);

	// Select data transmission
	MK_LIB_SET_DC_PIN(1);

	// Select display for SPI communication
	MK_LIB_SET_CS_PIN(0);

	/* send the colour data */
	for(uint16_t i = (IL3829_DISPLAY_WIDTH / 8) * IL3829_DISPLAY_HEIGHT;
		i != 0; i--)
	{
		MK_LIB_SEND_SPI_BYTE(color);
	}

	MK_LIB_SET_CS_PIN(1);
}

void disp_display_frame(void)
{
    send_disp_command(DISPLAY_UPDATE_CONTROL_2);
    const uint8_t duc2_data[] = { 0xC4 };
    send_spi_buffer(duc2_data, sizeof (duc2_data));
    
    send_disp_command(MASTER_ACTIVATION);
    send_disp_command(TERMINATE_FRAME_READ_WRITE);
    
    disp_wait_until_idle();
}

void disp_sleep(void)
{
	send_disp_command(DEEP_SLEEP_MODE);
	
	const uint8_t sleep_data[] = { 0x01 };
	send_spi_buffer(sleep_data, sizeof(sleep_data));
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
	MK_LIB_SET_DC_PIN(1);

	// Select display for SPI communication
	MK_LIB_SET_CS_PIN(0);

	/* send the pattern data */
	do {
		MK_LIB_SEND_SPI_BYTE(pattern);
	} while(--pixels);
	
	MK_LIB_SET_CS_PIN(1);
}

void disp_write_image(const uint8_t x, const uint8_t y,
		      const uint8_t x_end, const uint8_t y_end,
		      const uint8_t *img, const uint_fast16_t len)
{
	disp_set_memory_area(x, y, x_end, y_end);
	set_memory_pointer(x, y);
	send_disp_command(WRITE_RAM);

	// Select data transmission
	MK_LIB_SET_DC_PIN(1);

	// Select display for SPI communication
	MK_LIB_SET_CS_PIN(0);

	/* send the pattern data */
	// TODO: Possible optimisation?
	for(uint_fast16_t i = 0; i < len; i++)
		MK_LIB_SEND_SPI_BYTE(img[i]);

	MK_LIB_SET_CS_PIN(1);
}

void main(void)
{
	/* Initialise PIC microcontroller parameters. */
	init_pic();

	/* Initialise display. */
	init_display();

	// Initialise display frame buffer to white.
	disp_clear_frame_memory(0xFF);

	// Draw various sized boxes
	disp_set_block(0, 0, 0, 8, 0x00);
	disp_set_block(8, 8, 8, 16, 0x00);
	disp_set_block(16, 16, 24, 24, 0x00);
	
	// Draw thick 8 pixel line across the screen.
	disp_set_block(8*0, 56, 8*24, 64, 0x00);
	
	// Draw 4*8 boxes across the screen.
	disp_set_block(8*0, 64, 8*24, 72, 0xF0);
	
	// Draw 4*8 boxes across the screen in alternate colour.
	disp_set_block(8*0, 72, 8*24, 80, 0x0F);
	
	// Draw 2*8 boxes across the screen in alternate colour.
	disp_set_block(8*0, 80, 8*24, 88, 0b11001100);
	
	// Draw 1*8 boxes across the screen in alternate colour.
	disp_set_block(8*0, 88, 8*24, 96, 0b01010101);
	
	// Draw thick 5 pixel line
	disp_set_block(8*4, 100, 8*5, 105, 0x00);
	
	// Draw 1 pixel dotted line
	disp_set_block(8*4, 110, 8*4, 110, 0b01010101);
	
	// Draw 1 pixel line
	disp_set_block(8*4, 120, 8*5, 120, 0x00);
	
	// Draw 11 pixel dotted line
	disp_set_block(8*4, 130, 8*4, 141, 0b01010101);
	
	// Draw long vertical line
	disp_set_block(8*23, 10, 8*23, 190, 0b01111111);
	
	// Draw long horizontal line
	disp_set_block(8*1, 180, 8*23, 180, 0x00);
	
	// Draw another thick 8 pixel line across the screen.
	disp_set_block(8*0, 148, 8*24, 156, 0x00);
	
	// Draw the hello world bitmap
	disp_write_image(8*4, 110, (8*4)+hello_world_x, 110+hello_world_y,
			 hello_world_img, hello_world_len);
	
	// Draw the W bitmap
	disp_write_image(8*10, 102, (8*10)+w_x, 102+w_y, w_img, w_len);
	
	// Draw display frame buffer to screen
	disp_display_frame();
	
	// Set display to deep sleep mode
	disp_sleep();
	
	// Put microcontroller to sleep mode; stops restarting program.
	SLEEP();
	
	return;
}
