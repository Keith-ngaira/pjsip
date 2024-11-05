# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(pjproject)

# Set PJSIP source and library directories
set(PJSIP_SRC_DIR "C:/pjsip/pjsip")
set(PJSIP_LIB_DIR "C:/pjsip/build/lib")
link_directories(${PJSIP_LIB_DIR})

# Set OpenSSL root directory
set(OPENSSL_ROOT_DIR "C:/OpenSSL-Win64")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIB_DIR "${OPENSSL_ROOT_DIR}/lib")

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(
    ${PJSIP_SRC_DIR}/include
    ${OPENSSL_INCLUDE_DIR}
)

# Define PJSIP source files
file(GLOB_RECURSE PJSIP_SOURCES 
    "${PJSIP_SRC_DIR}/pjlib/*.c"
    "${PJSIP_SRC_DIR}/pjlib-util/*.c"
    "${PJSIP_SRC_DIR}/pjmedia/*.c"
    "${PJSIP_SRC_DIR}/pjmedia-audiodev/*.c"
    "${PJSIP_SRC_DIR}/pjmedia-codec/*.c"
    "${PJSIP_SRC_DIR}/pjmedia-videodev/*.c"
    "${PJSIP_SRC_DIR}/third_party/*.c"
    "${PJSIP_SRC_DIR}/pjsip/*.c"
    "${PJSIP_SRC_DIR}/pjsip-simple/*.c"
    "${PJSIP_SRC_DIR}/pjsip-ua/*.c"
    "${PJSIP_SRC_DIR}/pjsua-lib/*.c"
    "${PJSIP_SRC_DIR}/pjsua2/*.cpp"
)

# Add the executable and specify the source files
add_executable(pjproject main.cpp)

# Link libraries (including OpenSSL for SSL/TLS support)
target_link_libraries(pjproject
    OpenSSL::SSL
    OpenSSL::Crypto
    pjsua2
    pjmedia
    pjmedia-codec
    pjsip
    pjsip-simple
    pjsip-ua
    pj
)

# Ensure to link with the OpenSSL libraries correctly
if (WIN32)
    target_link_libraries(pjproject
        "${OPENSSL_LIB_DIR}/libssl.lib"
        "${OPENSSL_LIB_DIR}/libcrypto.lib"
    )
endif()
