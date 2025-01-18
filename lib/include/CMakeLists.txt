aux_source_directory(. SRC_LIST) 

add_library(cmrpc  ${SRC_LIST})

target_link_libraries(cmrpc muduo_base muduo_net pthread  zookeeper_mt)