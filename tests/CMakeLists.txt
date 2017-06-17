include_directories(${PROJECT_SOURCE_DIR}/include)
##################################################
set(THREAD_TEST_SRC ThreadTest.cpp)
add_executable(threadtest ${THREAD_TEST_SRC})
##################################################
set(STRONG_POINTER_TEST_SRC StrongPointerTest.cpp)
add_executable(strongpointerTest ${STRONG_POINTER_TEST_SRC})
##################################################
if (CMAKE_SYSTEM_NAME STREQUAL Linux)
target_link_libraries(
    threadtest
    vmodule
    pthread)
target_link_libraries(
    strongpointerTest
    vmodule
    pthread)    
elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
target_link_libraries(
    threadtest
    vmodule
    android
    log)
target_link_libraries(
    strongpointerTest
    vmodule
    android
    log)    
else()
endif()

