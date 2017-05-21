################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../sutils/Condition.cpp \
../sutils/Exception.cpp \
../sutils/ILogger.cpp \
../sutils/Logger.cpp \
../sutils/LoggerThread.cpp \
../sutils/Mutex.cpp \
../sutils/RefBase.cpp \
../sutils/StringUtils.cpp \
../sutils/Thread.cpp \
../sutils/ThreadImpl.cpp \
../sutils/Timers.cpp \
../sutils/VmBlockingQueue.cpp \
../sutils/VmHeap.cpp \
../sutils/VmQueue.cpp 

OBJS += \
./sutils/Condition.o \
./sutils/Exception.o \
./sutils/ILogger.o \
./sutils/Logger.o \
./sutils/LoggerThread.o \
./sutils/Mutex.o \
./sutils/RefBase.o \
./sutils/StringUtils.o \
./sutils/Thread.o \
./sutils/ThreadImpl.o \
./sutils/Timers.o \
./sutils/VmBlockingQueue.o \
./sutils/VmHeap.o \
./sutils/VmQueue.o 

CPP_DEPS += \
./sutils/Condition.d \
./sutils/Exception.d \
./sutils/ILogger.d \
./sutils/Logger.d \
./sutils/LoggerThread.d \
./sutils/Mutex.d \
./sutils/RefBase.d \
./sutils/StringUtils.d \
./sutils/Thread.d \
./sutils/ThreadImpl.d \
./sutils/Timers.d \
./sutils/VmBlockingQueue.d \
./sutils/VmHeap.d \
./sutils/VmQueue.d 


# Each subdirectory must supply rules for building sources it contributes
sutils/%.o: ../sutils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__cplusplus=201103L -I"/home/jeffrey/workSpace/workApp/vmodule/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -DTARGET_POSIX -DDEBUG_ENABLE -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


