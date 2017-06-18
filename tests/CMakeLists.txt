include_directories(${PROJECT_SOURCE_DIR}/include)
##################################################
set(THREAD_TEST_SRC ThreadTest.cpp)
add_executable(threadtest ${THREAD_TEST_SRC})
##################################################
set(STRONG_POINTER_TEST_SRC StrongPointerTest.cpp)
add_executable(strongpointerTest ${STRONG_POINTER_TEST_SRC})
##################################################

##################################################
set(QUEUE_TEST_SRC QueueTest.cpp)
add_executable(QueueTest ${QUEUE_TEST_SRC})
##################################################

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
target_link_libraries(
    threadtest
    sutils
    pthread)
target_link_libraries(
    strongpointerTest
    sutils
    pthread)   
target_link_libraries(
    QueueTest
    sutils
    pthread)       
elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
target_link_libraries(
    threadtest
    sutils
    android
    log)
target_link_libraries(
    strongpointerTest
    sutils
    android
    log)    
    
target_link_libraries(
    QueueTest
    sutils
    android
    log)       
else()
endif()

