
aux_source_directory(. LIB_SRCS)

add_library(LeaderLib ${LIB_SRCS} MessageQueue.cpp MessageQueue.h Message.h utils.h utils.cpp)