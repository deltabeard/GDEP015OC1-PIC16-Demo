/* 
 * Copyright (c) 2019, Mahyar Koshkouei
 * Released under ISC License.
 * 
 * Definitions for the GDEP015OC1 display.
 * 
 * Parts of this file is adapted from code found at:
 * https://github.com/MHEtLive/MH-ET-LIVE-E-Papers
 */

#ifndef GDEP015OC1_H
#define	GDEP015OC1_H

/* User required definitions. */
#ifndef MK_LIB_READ_BUSY_PIN
#error "MK_LIB_READ_BUSY_PIN was not defined."
#endif

#ifndef MK_LIB_SET_RESET_PIN
#error "MK_LIB_SET_RESET_PIN was not defined."
#endif

#ifndef MK_LIB_SET_DC_PIN
#error "MK_LIB_SET_DC_PIN was not defined."
#endif

#ifndef MK_LIB_SET_CS_PIN
#error "MK_LIB_SET_CS_PIN was not defined."
#endif

#ifndef MK_LIB_SET_SCLK_PIN
#error "MK_LIB_SET_SCLK_PIN was not defined."
#endif

#ifndef MK_LIB_SET_SDI_PIN
#error "MK_LIB_SET_SDI_PIN was not defined."
#endif

#ifndef MK_LIB_SEND_SPI_BYTE
#error "MK_LIB_SEND_SPI_BYTE was not defined."
#endif

/* Display resolution */
#define IL3829_DISPLAY_WIDTH	200
#define IL3829_DISPLAY_HEIGHT	200

// GDEP015OC1 commands
#define DRIVER_OUTPUT_CONTROL                       0x01
#define BOOSTER_SOFT_START_CONTROL                  0x0C
#define GATE_SCAN_START_POSITION                    0x0F
#define DEEP_SLEEP_MODE                             0x10
#define DATA_ENTRY_MODE_SETTING                     0x11
#define SW_RESET                                    0x12
#define TEMPERATURE_SENSOR_CONTROL                  0x1A
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_1                    0x21
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define WRITE_RAM                                   0x24
#define WRITE_VCOM_REGISTER                         0x2C
#define WRITE_LUT_REGISTER                          0x32
#define SET_DUMMY_LINE_PERIOD                       0x3A
#define SET_GATE_TIME                               0x3B
#define BORDER_WAVEFORM_CONTROL                     0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION        0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION        0x45
#define SET_RAM_X_ADDRESS_COUNTER                   0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                   0x4F
#define TERMINATE_FRAME_READ_WRITE                  0xFF

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

#endif	/* GDEP015OC1_H */
