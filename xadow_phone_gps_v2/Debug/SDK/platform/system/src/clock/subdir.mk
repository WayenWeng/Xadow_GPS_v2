################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDK/platform/system/src/clock/fsl_clock_manager.c \
../SDK/platform/system/src/clock/fsl_clock_manager_common.c 

OBJS += \
./SDK/platform/system/src/clock/fsl_clock_manager.o \
./SDK/platform/system/src/clock/fsl_clock_manager_common.o 

C_DEPS += \
./SDK/platform/system/src/clock/fsl_clock_manager.d \
./SDK/platform/system/src/clock/fsl_clock_manager_common.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/system/src/clock/%.o: ../SDK/platform/system/src/clock/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D"FSL_OSA_BM_TIMER_CONFIG=2" -D"CPU_MKL02Z32VFG4" -I"D:/Freescale/workspace/xadow_phone_gps_v2/SDK/platform/hal/inc" -I"D:/Freescale/workspace/xadow_phone_gps_v2/SDK/platform/hal/src/sim/MKL02Z4" -I"D:/Freescale/workspace/xadow_phone_gps_v2/SDK/platform/system/src/clock/MKL02Z4" -I"D:/Freescale/workspace/xadow_phone_gps_v2/SDK/platform/system/inc" -I"D:/Freescale/workspace/xadow_phone_gps_v2/SDK/platform/osa/inc" -I"D:/Freescale/workspace/xadow_phone_gps_v2/SDK/platform/CMSIS/Include" -I"D:/Freescale/workspace/xadow_phone_gps_v2/SDK/platform/devices" -I"D:/Freescale/workspace/xadow_phone_gps_v2/SDK/platform/devices/MKL02Z4/include" -I"D:/Freescale/workspace/xadow_phone_gps_v2/SDK/platform/devices/MKL02Z4/startup" -I"D:/Freescale/workspace/xadow_phone_gps_v2/Generated_Code/SDK/platform/devices/MKL02Z4/startup" -I"D:/Freescale/workspace/xadow_phone_gps_v2/Sources" -I"D:/Freescale/workspace/xadow_phone_gps_v2/Generated_Code" -I"D:/Freescale/workspace/xadow_phone_gps_v2/SDK/platform/utilities/inc" -I"D:/Freescale/workspace/xadow_phone_gps_v2/SDK/platform/drivers/inc" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


