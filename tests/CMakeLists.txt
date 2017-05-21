cmake_minimum_required(VERSION 3.4.1)
include_directories(${PROJECT_SOURCE_DIR}/include)
##################################################
set(THREAD_TEST_SRC ThreadTest.cpp)
add_executable(threadtest ${THREAD_TEST_SRC})
target_link_libraries(threadtest sutils)
##################################################
set(STRONG_POINTER_TEST_SRC StrongPointerTest.cpp)
add_executable(strongpointerTest ${STRONG_POINTER_TEST_SRC})
target_link_libraries(strongpointerTest sutils)
##################################################





