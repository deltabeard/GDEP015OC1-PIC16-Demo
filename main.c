/**
 * Copyright (c) 2019, Mahyar Koshkouei
 * Released under ISC License.
 * 
 * Displays Hello World image onto the MH-ET Live 1.54 E-paper Module, which
 * uses the GDEP015OC1 screen by Good Display, which uses the IL3829 display
 * driver.
 */

#define _XTAL_FREQ 1000000

#include <stdint.h>

#include "pic_config_bits.h"
#include <xc.h>

#define MK_GDEP_READ_BUSY_PIN		PORTBbits.RB0
#define MK_GDEP_RESET_DISPLAY()		reset_display()
#define MK_GDEP_SET_DC_PIN(x)		PORTBbits.RB2 = x
#define MK_GDEP_SET_CS_PIN(x)		PORTBbits.RB3 = x
#define MK_GDEP_SET_SCLK_PIN(x)		PORTBbits.RB4 = x
#define MK_GDEP_SET_SDI_PIN(x)		PORTBbits.RB5 = x
#define MK_GDEP_SEND_SPI_BYTE(x)	send_spi_byte(x)

inline void reset_display(void);
inline void send_spi_byte(const uint8_t byte);

#include "GDEP015OC1.h"


/* Bitmaps to draw. */
#include "w.h"
#include "hello_world.h"

#define DISPLAY_RESET_PIN(x)		PORTBbits.RB1 = x

inline void send_spi_byte(const uint8_t byte)
{
	uint_fast8_t unused_read;
	SSP1BUF = byte;
	while(!SSP1STATbits.BF);
	unused_read = SSP1BUF;
}

inline void reset_display(void)
{
	DISPLAY_RESET_PIN(0);
	NOP();
	DISPLAY_RESET_PIN(1);
	NOP();
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

	MK_GDEP_SET_CS_PIN(1);
}

void main(void)
{
	/* Initialise PIC microcontroller parameters. */
	init_pic();

	/* Initialise display. */
	init_display(LUT_FULL);

	// Initialise display frame buffer to white.
	disp_clear_frame_memory(0xFF);

	// Draw various sized boxes
	disp_set_block(0, 0, 0, 8, 0x00);
	disp_set_block(8, 8, 8, 16, 0x00);
	disp_set_block(16, 16, 24, 24, 0x00);

	// Draw thick 8 pixel line across the screen.
	disp_set_block(8 * 0, 56, 8 * 24, 64, 0x00);

	// Draw 4*8 boxes across the screen.
	disp_set_block(8 * 0, 64, 8 * 24, 72, 0xF0);

	// Draw 4*8 boxes across the screen in alternate colour.
	disp_set_block(8 * 0, 72, 8 * 24, 80, 0x0F);

	// Draw 2*8 boxes across the screen in alternate colour.
	disp_set_block(8 * 0, 80, 8 * 24, 88, 0b11001100);

	// Draw 1*8 boxes across the screen in alternate colour.
	disp_set_block(8 * 0, 88, 8 * 24, 96, 0b01010101);

	// Draw thick 5 pixel line
	disp_set_block(8 * 4, 100, 8 * 5, 105, 0x00);

	// Draw 1 pixel dotted line
	disp_set_block(8 * 4, 110, 8 * 4, 110, 0b01010101);

	// Draw 1 pixel line
	disp_set_block(8 * 4, 120, 8 * 5, 120, 0x00);

	// Draw 11 pixel dotted line
	disp_set_block(8 * 4, 130, 8 * 4, 141, 0b01010101);

	// Draw long vertical line
	disp_set_block(8 * 23, 10, 8 * 23, 190, 0b01111111);

	// Draw long horizontal line
	disp_set_block(8 * 1, 180, 8 * 23, 180, 0x00);

	// Draw another thick 8 pixel line across the screen.
	disp_set_block(8 * 0, 148, 8 * 24, 156, 0x00);

	// Draw the hello world bitmap
	disp_write_image(8 * 4, 110, (8 * 4) + hello_world_x, 110 + hello_world_y,
			hello_world_img, hello_world_len);

	// Draw the W bitmap
	disp_write_image(8 * 14, 102, (8 * 14) + w_x, 102 + w_y, w_img, w_len);

	// Draw display frame buffer to screen
	disp_display_frame();

	// Set display to deep sleep mode
	disp_sleep();

	// Put microcontroller to sleep mode; stops restarting program.
	SLEEP();

	return;
}
