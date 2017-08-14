file(GLOB PROTOS *.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_BINARY_DIR} ${PROTOS} )

add_executable(iUDPServer
  iUDPServer.cpp
  ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(iUDPServer
  goby_moos
  ${MOOS_LIBRARIES}
  ${MOOSGeodesy_LIBRARIES}
  ${Boost_LIBRARIES}
  ${PROTOBUF_LIBRARY}
  ${CMAKE_DL_LIBS}
  example_messages)
