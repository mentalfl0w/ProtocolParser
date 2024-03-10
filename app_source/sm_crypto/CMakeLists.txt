cmake_minimum_required(VERSION 3.21)

project(sm_crypto VERSION 1.0.0)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
    sources_files 
    src/big.c src/debug.c src/ecc.c src/inet.c src/random.c
    src/sm2.c src/sm3.c src/sm4.c
    include/big.h include/debug.h include/ecc.h include/inet.h
    include/random.h include/sm2.h include/sm3.h include/sm4.h
    include/typedef.h
)

INCLUDE_DIRECTORIES(include)

add_library(${PROJECT_NAME} STATIC ${sources_files})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(${PROJECT_NAME} PRIVATE SM_CRYPTO_LIBRARY)
