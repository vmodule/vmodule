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
    AutoBuffer.cpp
    FileUtils.cpp)

if(BUILD_SHARED_LIBS)    
add_library(sutils SHARED ${LIB_SUTILS_SRC})
else()
add_library(sutils STATIC ${LIB_SUTILS_SRC})
endif()

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
target_link_libraries(
    sutils 
    pthread)
elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
target_link_libraries(
    sutils 
    android
    log)
else()
endif()
