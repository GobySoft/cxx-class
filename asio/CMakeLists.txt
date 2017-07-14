project(multi_hop_comms)
set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.5)

find_package(DCCL REQUIRED)

get_target_property(DCCL_INCLUDE_DIR dccl INTERFACE_INCLUDE_DIRECTORIES)

add_custom_command(OUTPUT udp.pb.h udp.pb.cc
  COMMAND protoc --cpp_out=${CMAKE_CURRENT_BINARY_DIR} -I ${CMAKE_CURRENT_SOURCE_DIR} -I${DCCL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/udp.proto
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/udp.proto)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(drone drone.cpp udp.pb.h udp.pb.cc gpsreader.h)
add_executable(jetyak jetyak.cpp udp.pb.h udp.pb.cc)

target_link_libraries(drone boost_system protobuf dccl)
target_link_libraries(jetyak boost_system protobuf dccl)
