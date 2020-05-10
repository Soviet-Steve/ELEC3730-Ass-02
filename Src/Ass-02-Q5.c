/*        $Id: $
 *  $Revision: $
 *      $Date: $
 *    $Author: Peter $
 */

#include "Ass-02.h"
#include "adc.h"
// #define HEADPHONE
#ifdef HEADPHONE
  #include "headphoneDriver.h"
#else
  #include "dac.h"
#endif

#if DO_QUESTION == 5

typedef volatile enum{
    Safe = 0,
    Idle = 1,
    Grind = 2,
    PauseSet = 3,
    Pause = 4
} tenState;

tenState enState;

typedef enum tenButton{
  Pressed = 1,
  notPressed = 0
} enButton;

typedef enum tenGrind{
  Finished = 1,
  notFinished = 0
} enGrind;

#ifdef HEADPHONE
void fnvdStopGrind(void){
  return;
}

void fnvdStartGrind(void){
  HAL_I2S_Transmit(&hi2s2, (uint16_t *)&inarSound, 8192, 1000);
  return;
}
#else
void fnvdStopGrind(void){
  return;
}

void fnvdStartGrind(void){
  for(uint16_t i = 0; i < 8192; i++){
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_8B_R, inarSound[i]);
  }
  return;
}
#endif



enButton fnenDebounce(void){
  static int8_t inButton = 0;
  static enButton enState = notPressed;
  HAL_Delay(1); // While it adds a delay, 10ms of lag is acceptable (replace with osDelay in the future?)
  if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == 1){
    inButton++;
    if(inButton == 20){
      inButton = 0;
      enState = Pressed;
    }
  }
  if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == 0){
    inButton--;
      if(inButton){
        inButton = 0;
        enState = notPressed;
      }
  }
  return enState;
}

void fnPwmIdle(void){
    fnvdStopGrind();
    for(int i = 0; i < 10; i++){
    if(level == 2000){
      increament = -1;
    }
    if(level == 0){
      increament = 1;
    }
    level += increament;
    HAL_Delay(1);
    }
}


uint8_t fninReadADC(void){
  HAL_ADC_Start(&hadc1);
  if(HAL_ADC_PollForConversion(&hadc1, 8) == HAL_OK){ // Polling timeout only 8ms, if the ADC isn't ready at this stage, timing problems occur
    return (uint8_t)((HAL_ADC_GetValue(&hadc1) / 136) + 10); // Returns a number beteewn 10 and 40 for coffee grind duration
  }
  return 0; // Null coffee grind time
}

enGrind fnenGrind(const uint8_t inGrindTime){
  static uint8_t inCurrentTime = 0;
  if((inCurrentTime) < (inGrindTime * 1000) ){
    fnvdStartGrind();
    HAL_Delay(1);
    inCurrentTime++;
    return notFinished;
  }
  inCurrentTime = 0;
  return Finished;
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){ // Novel Extension -- Safety Switch
  enState = Safe;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, SET); // Turn on redled
  fnvdStopGrind();
}



volatile uint8_t start_loop = 0; // Flag to indicate start of loop

// Performance information
uint32_t loop_count = 0;
uint32_t wait_count;
uint32_t wait_count_min = 1 << 31;
uint32_t wait_count_max = 0;
uint32_t wait_count_avg = 0;

void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)
{
  if (htim->Instance == htim3.Instance)
  {
    // printf("INFO: TIM3 Callback function\n");
    start_loop = 1;
  }
}



void Ass_02_question (){
#ifdef HEADPHONES
  fnvdSetup1380();
  fnvd1380outOn();
#else
  HAL_DAC_Init(&hdac); // Start the dac interface
#endif
  uint8_t inGrindSeconds;
  // Initialise the timer and start running
  HAL_TIM_Base_Start_IT(&htim3);

  // Ignore first interrupt
  while (start_loop == 0);
  start_loop = 0;

  // Grinder application
  while (loop_count < 60000/TIMER_PERIOD) // Run for 1min assuming 10ms timer
  {
    // Wait for next loop
    if (start_loop == 1)
    {
      printf ("WARNING: Loop time exceeded\n");
    }
    wait_count = 0;
    while (start_loop == 0)
    {
      wait_count++;
    }
    start_loop = 0;

    // Performance monitoring
    loop_count++;
    wait_count_avg = (wait_count_avg+wait_count*(loop_count-1))/loop_count;
    if (wait_count > wait_count_max)
      wait_count_max = wait_count;
    if (wait_count < wait_count_min)
      wait_count_min = wait_count;

    // **** START COFFEE GRINDER MAIN LOOP CODE ****
#ifdef HEADPHONES
  fnvd1380setBothChannelVol(255); // This can be changed with a pot value if the end user wants it
#endif
    switch(enState){
      case Safe: // Don't have to worry about bouncing, because the interrupt will still take it back to this state
        if(HAL_GPIO_ReadPin(GPIOC, JOY_CTR_Pin) == 1){
          enState = Idle;
          HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, RESET); // Turn off red led
        }
        break;
      case Idle:
        fnPwmIdle();
        if(fnenDebounce() == Pressed){
           inGrindSeconds = fninReadADC();
           enState = Grind;
        }
        break;
      case Grind:
        if(fnenGrind(inGrindSeconds) == notFinished){
          if(fnenDebounce() == Pressed){
            enState = Pause;
          }
        }else{
          enState = Idle;
        }
        fnvdStopGrind(); // Always "stop the grind", because the grind will start again by the time it loops back around
        break;
      case PauseSet: // This prevents it from skipping the pause
        fnvdStopGrind();
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, SET); // Turn on the green LED
        if(fnenDebounce() == notPressed){
          enState = Pause;
        }
        break;
      case Pause:
        if(fnenDebounce() == Pressed){
         HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, RESET); // Turn off the green LED
        }
        break;
    }
    // ****  END COFFEE GRINDER MAIN LOOP CODE ****
  }

  // Print performance information
  printf("INFO: loop_count     = %ld (%fs)\n", loop_count, (float)loop_count*(float)TIMER_PERIOD/1000.0);
  printf("INFO: wait_count_max = %ld\n", wait_count_max);
  printf("INFO: wait_count_avg = %ld\n", wait_count_avg);
  printf("INFO: wait_count_min = %ld\n", wait_count_min);
}
#endif
