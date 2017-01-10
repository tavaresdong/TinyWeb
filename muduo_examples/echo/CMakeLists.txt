file(GLOB SRCS *.cc)

ADD_EXECUTABLE(echoServer ${SRCS})

TARGET_LINK_LIBRARIES(echoServer ${muduo_net})
TARGET_LINK_LIBRARIES(echoServer ${muduo_base})
TARGET_LINK_LIBRARIES(echoServer pthread rt)

