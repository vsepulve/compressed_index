cmake_minimum_required(VERSION 2.8.9)
project(compressed_index)
set(CMAKE_BUILD_TYPE Release)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O5")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -O5")
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(index_simple "${CMAKE_SOURCE_DIR}/programs/index_simple.cpp")
add_executable(generar_idf "${CMAKE_SOURCE_DIR}/programs/generar_idf.cpp")
add_executable(generar_docsize "${CMAKE_SOURCE_DIR}/programs/generar_docsize.cpp")
add_executable(generar_ubs "${CMAKE_SOURCE_DIR}/programs/generar_ubs.cpp")
add_executable(compress_index "${CMAKE_SOURCE_DIR}/programs/compress_index.cpp")

add_library(compressed_index SHARED ${SOURCES})
link_directories(${CMAKE_BINARY_DIR})

#target_link_libraries(decompress relz pthread sdsl divsufsort divsufsort64)
target_link_libraries(index_simple)
target_link_libraries(generar_idf compressed_index)
target_link_libraries(generar_docsize compressed_index)
target_link_libraries(generar_ubs compressed_index)
target_link_libraries(compress_index compressed_index)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/usr/" CACHE PATH "default install path" FORCE )
endif()

install(TARGETS compressed_index DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS index_simple DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/compressed_index FILES_MATCHING PATTERN "*.h")

