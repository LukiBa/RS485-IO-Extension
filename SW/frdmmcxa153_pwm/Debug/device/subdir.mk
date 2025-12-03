################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../device/system_MCXA153.c 

C_DEPS += \
./device/system_MCXA153.d 

OBJS += \
./device/system_MCXA153.o 


# Each subdirectory must supply rules for building sources it contributes
device/%.o: ../device/%.c device/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MCXA153VLH -DCPU_MCXA153VLH_cm33_nodsp -DMCUXPRESSO_SDK -DSDK_DEBUGCONSOLE=1 -DMCUX_META_BUILD -DMCXA153_SERIES -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\source" -I"C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\drivers" -I"C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\CMSIS" -I"C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\CMSIS\m-profile" -I"C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\device" -I"C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\device\periph" -I"C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\utilities" -I"C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\utilities\str" -I"C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\utilities\debug_console_lite" -I"C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\component\uart" -I"C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\board" -O0 -fno-common -g3 -gdwarf-4 -mcpu=cortex-m33+nodsp -c -ffunction-sections -fdata-sections -fno-builtin -imacros "C:\project\RS485-IO-Extension\SW\frdmmcxa153_pwm\source\mcux_config.h" -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33+nodsp -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-device

clean-device:
	-$(RM) ./device/system_MCXA153.d ./device/system_MCXA153.o

.PHONY: clean-device

