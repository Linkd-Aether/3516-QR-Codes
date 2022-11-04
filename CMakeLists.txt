cmake_minimum_required(VERSION 3.23.2)
project(3516-QR-Codes)

set(CMAKE_CXX_STANDARD 14)

add_executable(3516-QR-Codes QRServer.cpp)
add_executable(3516-QR-Codes_client QRClient.cpp)