################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/Ass-02-Q4.c \
../Src/Ass-02-Q5.c \
../Src/Ass-02.c \
../Src/adc.c \
../Src/dac.c \
../Src/dma.c \
../Src/gpio.c \
../Src/headphoneDriver.c \
../Src/i2c.c \
../Src/i2s.c \
../Src/main.c \
../Src/sound.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/system_stm32f4xx.c \
../Src/tim.c 

OBJS += \
./Src/Ass-02-Q4.o \
./Src/Ass-02-Q5.o \
./Src/Ass-02.o \
./Src/adc.o \
./Src/dac.o \
./Src/dma.o \
./Src/gpio.o \
./Src/headphoneDriver.o \
./Src/i2c.o \
./Src/i2s.o \
./Src/main.o \
./Src/sound.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/system_stm32f4xx.o \
./Src/tim.o 

C_DEPS += \
./Src/Ass-02-Q4.d \
./Src/Ass-02-Q5.d \
./Src/Ass-02.d \
./Src/adc.d \
./Src/dac.d \
./Src/dma.d \
./Src/gpio.d \
./Src/headphoneDriver.d \
./Src/i2c.d \
./Src/i2s.d \
./Src/main.d \
./Src/sound.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/system_stm32f4xx.d \
./Src/tim.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_HAL_DRIVER -DSTM32F407xx -I"C:/Users/benja/Documents/year2/sem1/elec3730/Ass-02/Inc" -I"C:/Users/benja/Documents/year2/sem1/elec3730/Ass-02/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/benja/Documents/year2/sem1/elec3730/Ass-02/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/benja/Documents/year2/sem1/elec3730/Ass-02/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/benja/Documents/year2/sem1/elec3730/Ass-02/Drivers/CMSIS/Include"  -O3 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


