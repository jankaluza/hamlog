FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(hamlog_server ${SRC})

#TARGET_LINK_LIBRARIES(hamlog_server)

INSTALL(TARGETS hamlog_server RUNTIME DESTINATION bin)


