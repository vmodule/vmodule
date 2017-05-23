cmake_minimum_required(VERSION 3.4.1)
include_directories(${PROJECT_SOURCE_DIR}/include)
##################################################
set(HAREWARE_SRC serial/SerialPort.cpp)
add_library(hardware SHARED ${HAREWARE_SRC})
target_link_libraries(hardware sutils)





