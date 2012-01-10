ADD_EXECUTABLE(parser_test parser_test.c)
TARGET_LINK_LIBRARIES(parser_test hamlog)

ADD_EXECUTABLE(list_test list_test.c)
TARGET_LINK_LIBRARIES(list_test hamlog)

ADD_EXECUTABLE(md5_test md5_test.c)
TARGET_LINK_LIBRARIES(md5_test hamlog)

ADD_EXECUTABLE(hash_test hash_test.c)
TARGET_LINK_LIBRARIES(hash_test hamlog)

