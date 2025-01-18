aux_source_directory(. SRC_LIST)

add_library(cmrpc ${SRC_LIST})

target_link_libraries(cmrpc muduo_net muduo_base  pthread zookeeper_mt )
