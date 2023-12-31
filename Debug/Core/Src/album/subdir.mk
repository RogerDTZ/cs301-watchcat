################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/album/exfuns.c \
../Core/Src/album/fattester.c \
../Core/Src/album/malloc.c \
../Core/Src/album/mmc_sd.c \
../Core/Src/album/piclib.c \
../Core/Src/album/remote.c \
../Core/Src/album/sys.c \
../Core/Src/album/tjpgd.c \
../Core/Src/album/w25qxx.c 

OBJS += \
./Core/Src/album/exfuns.o \
./Core/Src/album/fattester.o \
./Core/Src/album/malloc.o \
./Core/Src/album/mmc_sd.o \
./Core/Src/album/piclib.o \
./Core/Src/album/remote.o \
./Core/Src/album/sys.o \
./Core/Src/album/tjpgd.o \
./Core/Src/album/w25qxx.o 

C_DEPS += \
./Core/Src/album/exfuns.d \
./Core/Src/album/fattester.d \
./Core/Src/album/malloc.d \
./Core/Src/album/mmc_sd.d \
./Core/Src/album/piclib.d \
./Core/Src/album/remote.d \
./Core/Src/album/sys.d \
./Core/Src/album/tjpgd.d \
./Core/Src/album/w25qxx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/album/%.o Core/Src/album/%.su Core/Src/album/%.cyclo: ../Core/Src/album/%.c Core/Src/album/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/Cornelius/STM32CubeIDE/workspace_1.13.1/sdtest1/Core/Inc/album" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-album

clean-Core-2f-Src-2f-album:
	-$(RM) ./Core/Src/album/exfuns.cyclo ./Core/Src/album/exfuns.d ./Core/Src/album/exfuns.o ./Core/Src/album/exfuns.su ./Core/Src/album/fattester.cyclo ./Core/Src/album/fattester.d ./Core/Src/album/fattester.o ./Core/Src/album/fattester.su ./Core/Src/album/malloc.cyclo ./Core/Src/album/malloc.d ./Core/Src/album/malloc.o ./Core/Src/album/malloc.su ./Core/Src/album/mmc_sd.cyclo ./Core/Src/album/mmc_sd.d ./Core/Src/album/mmc_sd.o ./Core/Src/album/mmc_sd.su ./Core/Src/album/piclib.cyclo ./Core/Src/album/piclib.d ./Core/Src/album/piclib.o ./Core/Src/album/piclib.su ./Core/Src/album/remote.cyclo ./Core/Src/album/remote.d ./Core/Src/album/remote.o ./Core/Src/album/remote.su ./Core/Src/album/sys.cyclo ./Core/Src/album/sys.d ./Core/Src/album/sys.o ./Core/Src/album/sys.su ./Core/Src/album/tjpgd.cyclo ./Core/Src/album/tjpgd.d ./Core/Src/album/tjpgd.o ./Core/Src/album/tjpgd.su ./Core/Src/album/w25qxx.cyclo ./Core/Src/album/w25qxx.d ./Core/Src/album/w25qxx.o ./Core/Src/album/w25qxx.su

.PHONY: clean-Core-2f-Src-2f-album

