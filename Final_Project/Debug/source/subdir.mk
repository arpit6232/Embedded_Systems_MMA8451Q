################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/buffer.c \
../source/clock.c \
../source/i2c.c \
../source/i2carbiter.c \
../source/init_sensors.c \
../source/led.c \
../source/main.c \
../source/mma8451q.c \
../source/mtb.c \
../source/semihost_hardfault.c \
../source/statemachine.c \
../source/sysclock.c \
../source/systick.c 

OBJS += \
./source/buffer.o \
./source/clock.o \
./source/i2c.o \
./source/i2carbiter.o \
./source/init_sensors.o \
./source/led.o \
./source/main.o \
./source/mma8451q.o \
./source/mtb.o \
./source/semihost_hardfault.o \
./source/statemachine.o \
./source/sysclock.o \
./source/systick.o 

C_DEPS += \
./source/buffer.d \
./source/clock.d \
./source/i2c.d \
./source/i2carbiter.d \
./source/init_sensors.d \
./source/led.d \
./source/main.d \
./source/mma8451q.d \
./source/mtb.d \
./source/semihost_hardfault.d \
./source/statemachine.d \
./source/sysclock.d \
./source/systick.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL25Z128VLK4_cm0plus -DCPU_MKL25Z128VLK4 -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"/home/arpit/studies/pes/final/Final_Project/board" -I"/home/arpit/studies/pes/final/Final_Project/source" -I"/home/arpit/studies/pes/final/Final_Project" -I"/home/arpit/studies/pes/final/Final_Project/drivers" -I"/home/arpit/studies/pes/final/Final_Project/CMSIS" -I"/home/arpit/studies/pes/final/Final_Project/utilities" -I"/home/arpit/studies/pes/final/Final_Project/startup" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


