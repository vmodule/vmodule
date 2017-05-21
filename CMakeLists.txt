cmake_minimum_required(VERSION 3.4.1)
project(vmodule)
OPTION(BUILD_TESTS "Build test programs" OFF)
OPTION(BUILD_NETWORK "Build network programs" OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -UNDEBUG -g3 -O3 -Wall -fmessage-length=0 -Wno-unused-parameter -fexceptions")
add_definitions("-DDEBUG_ENABLE")
add_definitions("-DTARGET_POSIX")
SET(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/output)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output/libs)
#cmake ../ -DCMAKE_INSTALL_PREFIX=../output

include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(sutils)

if(BUILD_NETWORK)
add_subdirectory(network)
endif(BUILD_NETWORK)

set(LIB_VMODULE_SRC ${CMAKE_SOURCE_DIR}/vmodule.cpp)
add_executable(vmodule ${LIB_VMODULE_SRC})
target_link_libraries(
    vmodule 
    "-Wl,--whole-archive" #之后的库使用--whole-archive选项
    sutils
    pthread
    "-Wl,--no-whole-archive")  #之后的库不使用--whole-archive选项
#set_target_properties(vmodule_static PROPERTIES OUTPUT_NAME "vmodule")  

if(BUILD_TESTS)
add_subdirectory(tests)
endif(BUILD_TESTS)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ 
    DESTINATION include)   
     
INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/output/libs/ 
    DESTINATION libs
    PATTERN "libs/*"
    PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_WRITE WORLD_READ WORLD_WRITE) 

INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/output/bin/ 
    DESTINATION bin
    PATTERN "bin/*"
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ GROUP_WRITE WORLD_EXECUTE WORLD_READ WORLD_WRITE) 

