project(gps2)
set (CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.5)

find_package(DCCL REQUIRED)

get_target_property(DCCL_INCLUDE_DIR dccl INTERFACE_INCLUDE_DIRECTORIES)

add_custom_command(OUTPUT gps.pb.h gps.pb.cc
  COMMAND protoc --cpp_out=${CMAKE_CURRENT_BINARY_DIR}  -I ${CMAKE_CURRENT_SOURCE_DIR} -I ${DCCL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/gps.proto
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gps.proto)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
                 
add_executable(gps-parser2 gps-parser.cpp gpssentence.cpp gps.pb.h gps.pb.cc)

target_link_libraries(gps-parser2 protobuf dccl)
