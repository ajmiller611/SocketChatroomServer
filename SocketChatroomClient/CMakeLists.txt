set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
	client.cpp
	client.h
)

if(WIN32)
	set(LIBS ${LIBS} ws2_32)
endif()

# Add source to this project's executable.
add_executable (SocketChatroomClient ${SOURCES})

target_link_libraries(SocketChatroomClient PRIVATE ${LIBS})