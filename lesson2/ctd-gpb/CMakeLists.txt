project(ctd)
set (CMAKE_CXX_STANDARD 11)

find_package(DCCL REQUIRED)
get_target_property(DCCL_INCLUDE_DIR dccl INTERFACE_INCLUDE_DIRECTORIES)
add_custom_command(OUTPUT ctd.pb.h ctd.pb.cc
  COMMAND protoc --cpp_out=${CMAKE_CURRENT_BINARY_DIR}  -I ${CMAKE_CURRENT_SOURCE_DIR} -I ${DCCL_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/ctd.proto
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ctd.proto)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
                 
add_executable(ctd ctd.cpp ctd.pb.cc ctd.pb.h)
target_link_libraries(ctd protobuf dccl)
