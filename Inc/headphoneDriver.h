/**
 * Custom driver written by Benjamin Stanwell for the uda1380
 * @version 2
 */

#include "stm32f4xx.h"
#include "i2c.h"
#include "i2s.h"

#define UDA1380_I2C_ADDRESS 0x30

#define UDA1380_SETUP_REGISTER 0x00
#define UDA1380_I2S_FORMAT_REGISTER 0x01
#define UDA1380_PWR_REGISTER 0x02
#define UDA1380_MIXER_REGISTER 0x03
#define UDA1380_AMP_SETTING_REGISTER 0x04

#define UDA1380_MASTER_VOLUME_REGISTER 0x10
#define UDA1380_MIXER_VOLUME_REGISTER 0x11
#define UDA1380_FILTER_SELECT_REGISTER 0x12
#define UDA1380_MUTE_REGISTER 0x13
#define UDA1380_MIXER_OFF_REGISTER 0x14

#define UDA1380_DECIMATOR_VOLUME_CONTROL_REGISTER 0x20
#define UDA1380_PGA_SETTINGS_REGISTER 0x21
#define UDA1380_ADC_SETTINGS_REGISTER 0x22
#define UDA1380_AGC_SETTINGS_REGISTER 0x23

#define UDA1380_RESET_REGISTER 0x7F

#define UDA1380_OUTPUT_STATUS_REGISTER 0x18
#define UDA1380_INPUT_STATUS_REGISTER 0x28

/**
 * Shifts the config information into the audio chip
 */
void fnvdSetup1380(void);

/**
 * Turns on power for the output of the audio chip
 */ 
void fnvd1380outOn(void);

/**
 * Turns off power for the output of the audio chip
 */ 
void fnvd1380outOff(void);

/**
 * @par uint8Volume the gain for the heaphone port
 */
void fnvd1380setBothChannelVol(uint8_t uint8Volume);