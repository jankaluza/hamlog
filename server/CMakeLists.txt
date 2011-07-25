FILE(GLOB_RECURSE SRC *.cpp)

ADD_EXECUTABLE(hamlog_server ${SRC})
include_directories(.)

TARGET_LINK_LIBRARIES(hamlog_server ${Boost_LIBRARIES} ${SQLITE3_LIBRARIES} pthread)

INSTALL(TARGETS hamlog_server RUNTIME DESTINATION bin)


