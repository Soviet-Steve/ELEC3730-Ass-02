/*        $Id: $
 *  $Revision: $
 *      $Date: $
 *    $Author: Peter $
 */

// Common include file for all modules
#ifndef ASS_02_H_
#define ASS_02_H_

// Assignment and version strings
#define ASS_STRING "ELEC3730-2020s1 Assignment 2"
#define VER_STRING "Ver: 01 $Rev: XXXX $"

// Standard includes

extern void
initialise_monitor_handles (void); // STEPIEN: To allow printing to console
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Question to be completed

#define DO_QUESTION 4

// Uncomment as required

#include "tim.h"
// #include "adc.h"
// #include "dma.h"

// Timer loop period

#define TIMER_PERIOD 10 // ms

// LED colour definitions

#define LED_ORG LD3_Pin
#define LED_RED LD5_Pin
#define LED_BLU LD6_Pin
#define LED_GRN LD4_Pin

// Assignment functions

int
Ass_02_main ();
void
Ass_02_question ();

#endif /* ASS_02_H_ */
