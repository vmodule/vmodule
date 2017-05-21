################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../vmodule.cpp 

OBJS += \
./vmodule.o 

CPP_DEPS += \
./vmodule.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/jeffrey/workSpace/workApp/vmodule/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -DTARGET_POSIX -DDEBUG_ENABLE -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


