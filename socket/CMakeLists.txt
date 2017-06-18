include_directories(${PROJECT_SOURCE_DIR}/include)
set(SOCKET_SRC 
	Socket.cpp
	TcpSocket.cpp
	UdpSocket.cpp)
if(BUILD_SHARED_LIBS)    
add_library(csocket SHARED ${SOCKET_SRC})
else()
add_library(csocket STATIC ${SOCKET_SRC})
endif()
target_link_libraries(csocket sutils)
