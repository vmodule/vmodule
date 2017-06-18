cmake_minimum_required(VERSION 3.4.1 FATAL_ERROR)
cmake_policy(VERSION 3.4.1)
project(vmodule)
set(VMODULE_LIB_VERSION "0.0.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG -DDEBUG_ENABLE -std=c++11 -fexceptions")
option(BUILD_SHARED_LIBS "build as shared library" ON)
option(BUILD_STATIC_LIBS "build as static library" OFF)
option(BUILD_TESTS "build vmodule test" ON)
option(BUILD_SUTILS "build vmodule test" ON)
option(BUILD_NETWORK "Build network programs" OFF)
option(BUILD_HARDWARE "Build hareware programs" ON)

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
add_definitions("-DTARGET_POSIX")
elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
add_definitions("-DTARGET_ANDROID")
elseif(CMAKE_SYSTEM_NAME STREQUAL Windows)
add_definitions("-DTARGET_WINDOWS")
add_definitions("-DWIN32")
add_definitions("-D_CRT_SECURE_NO_WARNINGS")
else()
message("Unsupport platform here..")
endif()

message("CMAKE_SYSTEM_NAME :${CMAKE_SYSTEM_NAME}")

if(DEFINED CMAKE_INSTALL_PREFIX)
message("install path:${CMAKE_INSTALL_PREFIX}")
else()
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/output)
message("install path:${CMAKE_INSTALL_PREFIX}")
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_INSTALL_PREFIX}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_INSTALL_PREFIX}/libs)

if(BUILD_SUTILS)
add_subdirectory(sutils)
endif(BUILD_SUTILS)

if(BUILD_HARDWARE)
add_subdirectory(hardware)
endif(BUILD_HARDWARE)

if(BUILD_NETWORK)
add_subdirectory(network)
endif(BUILD_NETWORK)

#for libvmodule.
include_directories(${PROJECT_SOURCE_DIR}/include)
set(LIB_VMODULE_SRC
	${CMAKE_SOURCE_DIR}/vmodule.cpp)

if(BUILD_SHARED_LIBS) 
add_library(vmodule SHARED ${LIB_VMODULE_SRC})
else()
add_library(vmodule STATIC ${LIB_VMODULE_SRC})
endif()

if(BUILD_SUTILS)
target_link_libraries(
	vmodule 
	sutils
	)
endif()

if(BUILD_HARDWARE)
target_link_libraries(
	vmodule 
	"-Wl,--whole-archive"
	hardware
	"-Wl,--no-whole-archive"
	)
endif()

add_executable(
	vmoduleMain ${CMAKE_SOURCE_DIR}/main.cpp)

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
target_link_libraries(
    vmoduleMain 
    vmodule
    event
    pthread)
elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
#if the library not in system lib you must using flowing search
#if (BUILD_SHARED_LIBS)
#find_library(
#	event_lib 
#	libevent-2.1.so 
#	${CMAKE_SOURCE_DIR}/libs/android/)
#else()
#find_library(
#	event_lib 
#	libevent.a
#	${CMAKE_SOURCE_DIR}/libs/android/)
#endif()

target_link_libraries(
    vmoduleMain 
    vmodule
    event
    android
    log)
else()
endif()

#dependcy libvmodule..
if(BUILD_TESTS)
add_subdirectory(tests)
endif(BUILD_TESTS)    


INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ 
    DESTINATION include)   
     
INSTALL(DIRECTORY ${LIBRARY_OUTPUT_PATH}/ 
    DESTINATION libs
    PATTERN "libs/*"
    PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ GROUP_WRITE WORLD_READ 
WORLD_WRITE) 

INSTALL(DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/ 
    DESTINATION bin
    PATTERN "bin/*"
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ 
GROUP_WRITE WORLD_EXECUTE WORLD_READ WORLD_WRITE)
