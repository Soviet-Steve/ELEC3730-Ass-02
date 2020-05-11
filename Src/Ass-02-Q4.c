/*        $Id: $
 *  $Revision: $
 *      $Date: $
 *    $Author: Peter $
 */

#include "Ass-02.h"
#include "adc.h"
#include "sound.h"

// #define HEADPHONE

#ifdef HEADPHONE
  #include "headphoneDriver.h"
#else
  #include "dac.h"
#endif

#if DO_QUESTION == 4

volatile enum tenState{
    Safe = 0,
    Idle = 1,
    GrindSet = 2,
    Grind = 3,
    PauseSet = 4,
    Pause = 5
} enState;

// tenState enState;

typedef enum tenButton{
  Pressed = 1,
  notPressed = 0
} enButton;

typedef enum tenGrind{
  Finished = 1,
  notFinished = 0
} enGrind;

volatile struct tstrTimerInfo{
  uint16_t inMicroSecond;
  uint16_t inMilliSecond;
  uint16_t inSecond;
  uint8_t inGrinderTime;
  uint8_t inPauseTime;
} strRealTimer;

#ifdef HEADPHONE
void fnvdStopGrind(void){
  HAL_I2S_DMAPause(&hi2s2);
  return;
}

void fnvdStartGrind(void){
  HAL_I2S_DMAResume(&hi2s2);
  return;
}
#else
void fnvdStopGrind(void){
  HAL_GPIO_WritePin(GPIOD, LD3_Pin, RESET);
  HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
  return;
}

void fnvdStartGrind(void){
  HAL_GPIO_WritePin(GPIOD, LD3_Pin, SET);
  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)&inarSound, 128, DAC_ALIGN_12B_R);
  return;
}
#endif



enButton fnenDebounce(void){
  static int8_t inButton = 0, inPrevState = 0;
  static int64_t inHistory = 0;
  static enButton enState = notPressed;
 // HAL_Delay(1);
  if(HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO_Port, BLUE_BUTTON_Pin) == 1){
   // printf("Button is pressed\n");
    inButton = 1;
  }else{
   // printf("Button is not pressed\n");
    inButton = -1;
  }
  inHistory += (30 * inButton) - (10 * inPrevState);
  // printf("%d\n", inHistory);
  if(inHistory >= 100){
    inHistory = 100; // To prevent wrap around
    enState = Pressed;
    // printf("Button is pressed\n");
  }
  if(inHistory <= -100){
    inHistory = -100;
    enState = notPressed;
   // printf("Button is not Pressed\n");
  }
  inPrevState = inButton;
  return enState;
}

TIM_OC_InitTypeDef strPwm; // Stores all the information about the pwm to "input" into the pwm function


void fnPwmIdle(void){
    //HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, SET);
    //return;
    strPwm.OCMode = TIM_OCMODE_PWM1; // Set pwm mode to 1
    strPwm.OCPolarity = TIM_OCPOLARITY_HIGH; // Non inverted pwm
    strPwm.OCFastMode = TIM_OCFAST_DISABLE; // Fast mode??
    fnvdStopGrind();
    static uint16_t level = 1; // The code for the pwm "waving" comes from the waveshare pwm example
    static int16_t increament = 1;
    if(increament >= 2048){
      increament = -increament;
    }
    level = abs(increament += 8);
    strPwm.Pulse = level;
    HAL_TIM_PWM_ConfigChannel(&htim4, &strPwm, TIM_CHANNEL_4); // "Uploads the information into a temp register
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4); // Moves that temp register forward
}

void fnvdBlueOff(void){
  strPwm.Pulse = 0;
  HAL_TIM_PWM_ConfigChannel(&htim4, &strPwm, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

uint8_t fninReadADC(void){
  HAL_ADC_Start(&hadc1);
  if(HAL_ADC_PollForConversion(&hadc1, 8) == HAL_OK){ // Polling timeout only 8ms, if the ADC isn't ready at this stage, timing problems occur
    return ((HAL_ADC_GetValue(&hadc1) / 25) + 5); // Returns a number beteewn 10 and 40 for coffee grind duration
  }
  return 0; // Null coffee grind time
}

enGrind fnenGrind(void){
  if(strRealTimer.inMilliSecond == 0){
    strRealTimer.inGrinderTime--;
  }
  if(strRealTimer.inGrinderTime > 0){
    return notFinished;
  }
  return Finished;
}

void fnvdKill(void){
  fnvdStopGrind();
  fnvdBlueOff();
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, RESET); // Turn off the green LED
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, RESET);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, RESET);
}

void fnvdPauseTime(void){
  if(strRealTimer.inMilliSecond == 0){
    strRealTimer.inPauseTime--;
  }
  if(strRealTimer.inPauseTime <= 0){
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, RESET); // Turn off the green LED
    enState = Idle;
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){ // Novel Extension -- Safety Switch
  //printf("Going into Safe State\n");
  enState = Safe;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, RESET); // Turn off the green LED
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, SET); // Turn on redled
  fnvdBlueOff();
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
  strRealTimer.inMilliSecond += 10;
  if(strRealTimer.inMilliSecond == 1000){
    //printf("%d\n", strRealTimer.inPauseTime);
    //strRealTimer.inPauseTime--;
    strRealTimer.inMilliSecond = 0;
    strRealTimer.inSecond++;
  }
}




void Ass_02_question (){
#ifdef HEADPHONE
  fnvdSetup1380();
  fnvd1380outOn();
  HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t *)&inarSound, 128); // Has to be cast to a 16 bit int
  HAL_I2S_DMAPause(&hi2s2);
#else
   HAL_DAC_Init(&hdac); // Start the dac interface
   HAL_TIM_Base_Start(&htim2);
#endif
  strRealTimer.inSecond = 0;
  strRealTimer.inMilliSecond = 0;
  strRealTimer.inSecond = 0;
  uint8_t inGrindSeconds = 0;
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
#ifdef HEADPHONE
  fnvd1380setBothChannelVol(255); // This can be changed with a pot value if the end user wants it
#endif
    switch(enState){
      case Safe: // Don't have to worry about bouncing, because the interrupt will still take it back to this state
        if(HAL_GPIO_ReadPin(GPIOC, JOY_CTR_Pin) == 1){
          enState = Idle;
          // printf("Going into Idle State\n");
          HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, RESET);
        }
        break;
      case Idle:
        strRealTimer.inPauseTime = 0;
        fnPwmIdle();
        if(fnenDebounce() == Pressed){
          fnvdBlueOff();
          strRealTimer.inGrinderTime = fninReadADC();
         // printf("Going into GrindSet State\n");
        // printf("inGrindSeconds: %d\n", strRealTimer.inGrinderTime);
          enState = GrindSet;
        }
        break;
      case GrindSet:
        if(fnenDebounce() == notPressed){
         // printf("Going into Grind State\n");
          enState = Grind;
          fnvdStartGrind();
        }
        break;
      case Grind:
        if(fnenGrind() == notFinished){
          if(fnenDebounce() == Pressed){
           // printf("Going into PauseSet State\n");
            enState = PauseSet;
          }
        }else{
          fnvdStopGrind();
          enState = Idle;
        }

        break;
      case PauseSet: // This prevents it from skipping the pause;
        fnvdStopGrind();
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, SET); // Turn on the green LED
        if(fnenDebounce() == notPressed){
          //printf("Going into Pause State\n");
          strRealTimer.inPauseTime = 20;
          enState = Pause;
        }
        break;
      case Pause:
        // printf("%d\n", strRealTimer.inPauseTime);
        fnvdPauseTime();
        if(fnenDebounce() == Pressed){
         HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, RESET); // Turn off the green LED         
        // printf("Going into GrindSet State\n");
         enState = GrindSet;
        }
        break;
    }
    // ****  END COFFEE GRINDER MAIN LOOP CODE ****
  }
  fnvdKill();
  // Print performance information
  printf("INFO: loop_count     = %ld (%fs)\n", loop_count, (float)loop_count*(float)TIMER_PERIOD/1000.0);
  printf("INFO: wait_count_max = %ld\n", wait_count_max);
  printf("INFO: wait_count_avg = %ld\n", wait_count_avg);
  printf("INFO: wait_count_min = %ld\n", wait_count_min);
}

#endif
