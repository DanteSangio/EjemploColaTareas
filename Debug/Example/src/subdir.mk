################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Example/src/FRTOS.c \
../Example/src/cr_startup_lpc175x_6x.c \
../Example/src/crp.c \
../Example/src/sysinit.c 

OBJS += \
./Example/src/FRTOS.o \
./Example/src/cr_startup_lpc175x_6x.o \
./Example/src/crp.o \
./Example/src/sysinit.o 

C_DEPS += \
./Example/src/FRTOS.d \
./Example/src/cr_startup_lpc175x_6x.d \
./Example/src/crp.d \
./Example/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
Example/src/%.o: ../Example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_LPCOPEN -D__LPC17XX__ -D__REDLIB__ -I"C:\Users\unknown\Documents\LPCWorkspace\lpc_chip_175x_6x\src" -I"C:\Users\unknown\Documents\LPCWorkspace\lpc_board_nxp_lpcxpresso_1769" -I"C:\Users\unknown\Documents\LPCWorkspace\lpc_board_nxp_lpcxpresso_1769\inc" -I"C:\Users\unknown\Documents\LPCWorkspace\lpc_chip_175x_6x" -I"C:\Users\unknown\Documents\LPCWorkspace\PruebaPantallaILI2\Example\inc" -I"C:\Users\unknown\Documents\LPCWorkspace\lpc_chip_175x_6x\inc" -I"C:\Users\unknown\Documents\LPCWorkspace\PruebaPantallaILI2\freeRTOS\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


