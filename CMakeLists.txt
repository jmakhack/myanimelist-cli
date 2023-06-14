cmake_minimum_required(VERSION 3.12)
project(mya)
set(CMAKE_C_STANDARD 11 )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")

if(UNIX)
	set(LINUX TRUE)
endif()

set(SRC_DIR src)
set(INC_DIR include)

add_executable(mya ${SRC_DIR}/mya.c)

set_target_properties(mya PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

target_include_directories(mya PRIVATE ${INC_DIR})

set(LIBS curl json-c bsd)
if(LINUX)
    list(APPEND LIBS)
endif()
target_link_libraries(mya ${LIBS})
