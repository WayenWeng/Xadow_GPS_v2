################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../SDK/platform/devices/MKL02Z4/startup/gcc/startup_MKL02Z4.S 

OBJS += \
./SDK/platform/devices/MKL02Z4/startup/gcc/startup_MKL02Z4.o 

S_UPPER_DEPS += \
./SDK/platform/devices/MKL02Z4/startup/gcc/startup_MKL02Z4.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/devices/MKL02Z4/startup/gcc/%.o: ../SDK/platform/devices/MKL02Z4/startup/gcc/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -x assembler-with-cpp -D"FSL_OSA_BM_TIMER_CONFIG=2" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


