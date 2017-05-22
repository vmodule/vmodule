cmake_minimum_required(VERSION 3.4.1)
include_directories(${PROJECT_SOURCE_DIR}/include)
set(LIB_SUTILS_SRC
    Condition.cpp 
    Exception.cpp
    ILogger.cpp
    Logger.cpp
    LoggerThread.cpp
    Mutex.cpp
    RefBase.cpp
    StringUtils.cpp
    Thread.cpp
    ThreadImpl.cpp
    Timers.cpp
    VmBlockingQueue.cpp
    VmHeap.cpp
    VmQueue.cpp
    AutoBuffer.cpp)    
add_library(sutils SHARED ${LIB_SUTILS_SRC})
target_link_libraries(sutils
    pthread)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output/libs)
#set_target_properties(sutils_static PROPERTIES OUTPUT_NAME "sutils")
