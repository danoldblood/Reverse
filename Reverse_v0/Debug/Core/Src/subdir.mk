################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/allpass.c \
../Core/Src/delay.c \
../Core/Src/delay_modular.c \
../Core/Src/delay_mt.c \
../Core/Src/delay_parting.c \
../Core/Src/delay_parting_16bit.c \
../Core/Src/led.c \
../Core/Src/main.c \
../Core/Src/pitch.c \
../Core/Src/pot.c \
../Core/Src/statevariable.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/switchDB.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h7xx.c 

OBJS += \
./Core/Src/allpass.o \
./Core/Src/delay.o \
./Core/Src/delay_modular.o \
./Core/Src/delay_mt.o \
./Core/Src/delay_parting.o \
./Core/Src/delay_parting_16bit.o \
./Core/Src/led.o \
./Core/Src/main.o \
./Core/Src/pitch.o \
./Core/Src/pot.o \
./Core/Src/statevariable.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/switchDB.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h7xx.o 

C_DEPS += \
./Core/Src/allpass.d \
./Core/Src/delay.d \
./Core/Src/delay_modular.d \
./Core/Src/delay_mt.d \
./Core/Src/delay_parting.d \
./Core/Src/delay_parting_16bit.d \
./Core/Src/led.d \
./Core/Src/main.d \
./Core/Src/pitch.d \
./Core/Src/pot.d \
./Core/Src/statevariable.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/switchDB.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h7xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM7 -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I"/Users/danspechacek/git/EmRussGlitch/TwoWolves_Parting_v0pt0/Drivers/CMSIS/DSP/Include" -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -Os -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=softfp -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/allpass.cyclo ./Core/Src/allpass.d ./Core/Src/allpass.o ./Core/Src/allpass.su ./Core/Src/delay.cyclo ./Core/Src/delay.d ./Core/Src/delay.o ./Core/Src/delay.su ./Core/Src/delay_modular.cyclo ./Core/Src/delay_modular.d ./Core/Src/delay_modular.o ./Core/Src/delay_modular.su ./Core/Src/delay_mt.cyclo ./Core/Src/delay_mt.d ./Core/Src/delay_mt.o ./Core/Src/delay_mt.su ./Core/Src/delay_parting.cyclo ./Core/Src/delay_parting.d ./Core/Src/delay_parting.o ./Core/Src/delay_parting.su ./Core/Src/delay_parting_16bit.cyclo ./Core/Src/delay_parting_16bit.d ./Core/Src/delay_parting_16bit.o ./Core/Src/delay_parting_16bit.su ./Core/Src/led.cyclo ./Core/Src/led.d ./Core/Src/led.o ./Core/Src/led.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/pitch.cyclo ./Core/Src/pitch.d ./Core/Src/pitch.o ./Core/Src/pitch.su ./Core/Src/pot.cyclo ./Core/Src/pot.d ./Core/Src/pot.o ./Core/Src/pot.su ./Core/Src/statevariable.cyclo ./Core/Src/statevariable.d ./Core/Src/statevariable.o ./Core/Src/statevariable.su ./Core/Src/stm32h7xx_hal_msp.cyclo ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_hal_msp.su ./Core/Src/stm32h7xx_it.cyclo ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/stm32h7xx_it.su ./Core/Src/switchDB.cyclo ./Core/Src/switchDB.d ./Core/Src/switchDB.o ./Core/Src/switchDB.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32h7xx.cyclo ./Core/Src/system_stm32h7xx.d ./Core/Src/system_stm32h7xx.o ./Core/Src/system_stm32h7xx.su

.PHONY: clean-Core-2f-Src

