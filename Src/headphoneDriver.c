/**
 * Custom driver written by Benjamin Stanwell for the uda1380
 * @version 2
 */

#include "headphoneDriver.h"

void fnvdSetup1380(void){
    uint8_t unit8Data[8][3] = {
        {UDA1380_SETUP_REGISTER, 0x03, 0x12}, // All DAC clk on, no ADC dac
        {UDA1380_I2S_FORMAT_REGISTER, 0x00, 0x00}, // Normal I2S
        {UDA1380_MIXER_REGISTER, 0x00, 0x00}, // Full gain, all volume will go into the master mixer
        {UDA1380_AMP_SETTING_REGISTER, 0x02, 0x02}, // Default values and short circuit enabled
        {UDA1380_MIXER_VOLUME_REGISTER, 0x00, 0x00}, // Full Volume
        {UDA1380_FILTER_SELECT_REGISTER, 0x00, 0x00}, // No changes to treble or bass
        {UDA1380_MUTE_REGISTER, 0x00, 0x00}, // No mute
        {UDA1380_MIXER_OFF_REGISTER, 0x00, 0x00} // Normal settings
        }; 
    for(uint8_t i = 0; i < 8; i++){
        HAL_I2C_Master_Transmit(&hi2c1, UDA1380_I2C_ADDRESS, unit8Data[i], 3, 1000);
    }
    return;
}

void fnvd1380outOn(void){
    uint8_t unit8Data[3] = {UDA1380_PWR_REGISTER, 0xA5, 0xB0};
    HAL_I2C_Master_Transmit(&hi2c1, UDA1380_I2C_ADDRESS, unit8Data, 3, 1000);
    return;
}

void fnvd1380outOff(void){
    uint8_t unit8Data[3] = {UDA1380_PWR_REGISTER, 0xA5, 0xC0}; // Only turns on the DAC part of the chip, not the ADC
    HAL_I2C_Master_Transmit(&hi2c1, UDA1380_I2C_ADDRESS, unit8Data, 3, 1000);
    return;
}

void fnvd1380setBothChannelVol(uint8_t uint8Volume){
    uint8_t uint8Data[3] = {UDA1380_MASTER_VOLUME_REGISTER, 0xFF - uint8Volume, 0xFF - uint8Volume};
    HAL_I2C_Master_Transmit(&hi2c1, UDA1380_I2C_ADDRESS, uint8Data, 3, 1000);
    return;
}
