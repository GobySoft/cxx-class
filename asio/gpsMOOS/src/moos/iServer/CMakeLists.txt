file(GLOB PROTOS *.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_BINARY_DIR} ${PROTOS} )

add_executable(iServer
  iServer.cpp
  ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(iServer
  goby_moos
  ${MOOS_LIBRARIES}
  ${MOOSGeodesy_LIBRARIES}
  ${Boost_LIBRARIES}
  ${PROTOBUF_LIBRARY}
  ${CMAKE_DL_LIBS}
  example_messages)
