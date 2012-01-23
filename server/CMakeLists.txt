ADD_SUBDIRECTORY(responders)

FILE(GLOB SRC *.cpp storagebackends/*.cpp)

ADD_EXECUTABLE(hamlog_server ${SRC})
include_directories(.)

TARGET_LINK_LIBRARIES(hamlog_server ${Boost_LIBRARIES} ${SQLITE3_LIBRARIES} ${LOG4CXX_LIBRARIES} pthread dl)

INSTALL(TARGETS hamlog_server RUNTIME DESTINATION bin)


