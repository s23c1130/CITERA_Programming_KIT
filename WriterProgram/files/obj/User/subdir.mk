################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v203_mylib.c \
../User/ch32v20x_it.c \
../User/main.c \
../User/system_ch32v20x.c 

C_DEPS += \
./User/ch32v203_mylib.d \
./User/ch32v20x_it.d \
./User/main.d \
./User/system_ch32v20x.d 

OBJS += \
./User/ch32v203_mylib.o \
./User/ch32v20x_it.o \
./User/main.o \
./User/system_ch32v20x.o 


EXPANDS += \
./User/ch32v203_mylib.c.234r.expand \
./User/ch32v20x_it.c.234r.expand \
./User/main.c.234r.expand \
./User/system_ch32v20x.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	"../../tools/RISC-V Embedded GCC/bin/riscv-none-embed-gcc" -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I "../Debug" -I "../Core" -I "../User" -I "../Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

