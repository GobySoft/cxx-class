include_directories("/home/jvw/lamss/lamss/include")
link_directories("/home/jvw/lamss/lamss/lib")

file(GLOB PROTOS *.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_BINARY_DIR} ${PROTOS} )

add_executable(pLAMSSCatcher
  pLAMSSCatcher.cpp
  ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(pLAMSSCatcher
  goby_moos
  ${MOOS_LIBRARIES}
  ${MOOSGeodesy_LIBRARIES}
  ${Boost_LIBRARIES}
  ${PROTOBUF_LIBRARY}
  ${CMAKE_DL_LIBS}
  example_messages)
