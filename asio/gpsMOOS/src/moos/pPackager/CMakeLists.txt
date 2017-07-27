file(GLOB PROTOS *.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_BINARY_DIR} ${PROTOS} )

include_directories("/home/ltencate/lamss/include")
link_directories("/home/ltencate/lamss/lib")

add_executable(pPackager
  pPackager.cpp
  ${PROTO_SRCS} ${PROTO_HDRS})


target_link_libraries(pPackager
  goby_moos
  ${MOOS_LIBRARIES}
  ${MOOSGeodesy_LIBRARIES}
  ${Boost_LIBRARIES}
  ${PROTOBUF_LIBRARY}
  ${CMAKE_DL_LIBS}
  example_messages
  lamss_protobuf)
