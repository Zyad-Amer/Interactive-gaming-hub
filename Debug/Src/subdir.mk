################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/FrameBuffer.c \
../Src/HMATH_program.c \
../Src/HMEM_program.c \
../Src/HMENU_program.c \
../Src/HTFT_prg.c \
../Src/HXO_program.c \
../Src/MEXTI_Program.c \
../Src/MGPIO_prg.c \
../Src/MNVIC_Program.c \
../Src/MRCC_prg.c \
../Src/MSPI_prg.c \
../Src/MSTK_prg.c \
../Src/main.c \
../Src/snake_prg.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/FrameBuffer.o \
./Src/HMATH_program.o \
./Src/HMEM_program.o \
./Src/HMENU_program.o \
./Src/HTFT_prg.o \
./Src/HXO_program.o \
./Src/MEXTI_Program.o \
./Src/MGPIO_prg.o \
./Src/MNVIC_Program.o \
./Src/MRCC_prg.o \
./Src/MSPI_prg.o \
./Src/MSTK_prg.o \
./Src/main.o \
./Src/snake_prg.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/FrameBuffer.d \
./Src/HMATH_program.d \
./Src/HMEM_program.d \
./Src/HMENU_program.d \
./Src/HTFT_prg.d \
./Src/HXO_program.d \
./Src/MEXTI_Program.d \
./Src/MGPIO_prg.d \
./Src/MNVIC_Program.d \
./Src/MRCC_prg.d \
./Src/MSPI_prg.d \
./Src/MSTK_prg.d \
./Src/main.d \
./Src/snake_prg.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F401CCUx -DSTM32 -DSTM32F4 -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/FrameBuffer.cyclo ./Src/FrameBuffer.d ./Src/FrameBuffer.o ./Src/FrameBuffer.su ./Src/HMATH_program.cyclo ./Src/HMATH_program.d ./Src/HMATH_program.o ./Src/HMATH_program.su ./Src/HMEM_program.cyclo ./Src/HMEM_program.d ./Src/HMEM_program.o ./Src/HMEM_program.su ./Src/HMENU_program.cyclo ./Src/HMENU_program.d ./Src/HMENU_program.o ./Src/HMENU_program.su ./Src/HTFT_prg.cyclo ./Src/HTFT_prg.d ./Src/HTFT_prg.o ./Src/HTFT_prg.su ./Src/HXO_program.cyclo ./Src/HXO_program.d ./Src/HXO_program.o ./Src/HXO_program.su ./Src/MEXTI_Program.cyclo ./Src/MEXTI_Program.d ./Src/MEXTI_Program.o ./Src/MEXTI_Program.su ./Src/MGPIO_prg.cyclo ./Src/MGPIO_prg.d ./Src/MGPIO_prg.o ./Src/MGPIO_prg.su ./Src/MNVIC_Program.cyclo ./Src/MNVIC_Program.d ./Src/MNVIC_Program.o ./Src/MNVIC_Program.su ./Src/MRCC_prg.cyclo ./Src/MRCC_prg.d ./Src/MRCC_prg.o ./Src/MRCC_prg.su ./Src/MSPI_prg.cyclo ./Src/MSPI_prg.d ./Src/MSPI_prg.o ./Src/MSPI_prg.su ./Src/MSTK_prg.cyclo ./Src/MSTK_prg.d ./Src/MSTK_prg.o ./Src/MSTK_prg.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/snake_prg.cyclo ./Src/snake_prg.d ./Src/snake_prg.o ./Src/snake_prg.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

