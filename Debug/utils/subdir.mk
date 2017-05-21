################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../utils/Condition.cpp \
../utils/Mutex.cpp \
../utils/RefBase.cpp \
../utils/Timers.cpp 

OBJS += \
./utils/Condition.o \
./utils/Mutex.o \
./utils/RefBase.o \
./utils/Timers.o 

CPP_DEPS += \
./utils/Condition.d \
./utils/Mutex.d \
./utils/RefBase.d \
./utils/Timers.d 


# Each subdirectory must supply rules for building sources it contributes
utils/%.o: ../utils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/jeffrey/workSpace/workApp/vmodule/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -D__POSIX__ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


