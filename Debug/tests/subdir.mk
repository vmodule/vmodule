################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tests/StrongPointerTest.cpp \
../tests/ThreadTest.cpp 

OBJS += \
./tests/StrongPointerTest.o \
./tests/ThreadTest.o 

CPP_DEPS += \
./tests/StrongPointerTest.d \
./tests/ThreadTest.d 


# Each subdirectory must supply rules for building sources it contributes
tests/%.o: ../tests/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/jeffrey/workSpace/workApp/vmodule/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -DTARGET_POSIX -DDEBUG_ENABLE -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


