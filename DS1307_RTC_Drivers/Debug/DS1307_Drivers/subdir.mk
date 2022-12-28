################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DS1307_Drivers/DS1307_RTC.c 

OBJS += \
./DS1307_Drivers/DS1307_RTC.o 

C_DEPS += \
./DS1307_Drivers/DS1307_RTC.d 


# Each subdirectory must supply rules for building sources it contributes
DS1307_Drivers/%.o DS1307_Drivers/%.su: ../DS1307_Drivers/%.c DS1307_Drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I"D:/Resources/GIT/g_DS1307_RTC_Drivers/DS1307_RTC/DS1307_RTC_Drivers/DS1307_Drivers" -I../Inc -I"D:/Resources/GIT/g_DS1307_RTC_Drivers/DS1307_RTC/DS1307_RTC_Drivers/Device_Drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DS1307_Drivers

clean-DS1307_Drivers:
	-$(RM) ./DS1307_Drivers/DS1307_RTC.d ./DS1307_Drivers/DS1307_RTC.o ./DS1307_Drivers/DS1307_RTC.su

.PHONY: clean-DS1307_Drivers

