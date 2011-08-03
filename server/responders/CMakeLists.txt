cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-fPIC)
include_directories(..)

macro(BUILD_MODULE NAME SRC HDR)
	ADD_LIBRARY(${NAME} SHARED ${SRC} ${HDR})
	INSTALL(TARGETS ${NAME} LIBRARY DESTINATION lib/hamlog-server ARCHIVE DESTINATION lib/hamlog-server COMPONENT libraries)
	SET_TARGET_PROPERTIES(${NAME} PROPERTIES VERSION ${HAMLOG_VERSION} SOVERSION ${HAMLOG_VERSION})
endmacro(BUILD_MODULE)

BUILD_MODULE(mod_login login.cpp login.h)
BUILD_MODULE(mod_register register.cpp register.h)
BUILD_MODULE(mod_root root.cpp root.h)
BUILD_MODULE(mod_logbook logbook.cpp logbook.h)
