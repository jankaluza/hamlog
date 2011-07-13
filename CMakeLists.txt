cmake_minimum_required(VERSION 2.6)
project(hamlog)

set(CMAKE_MODULE_PATH "cmake_modules")

set(sqlite3_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(sqlite3)

set(Boost_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(Boost COMPONENTS date_time system filesystem program_options regex thread signals REQUIRED)

find_package(Doxygen)

INCLUDE(FindQt4)
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui)

# ADD_DEFINITIONS(${SWIFTEN_CFLAGS})

message("  Supported features")
message("-----------------------")
if (SQLITE3_FOUND)
	ADD_DEFINITIONS(-DWITH_SQLITE)
	include_directories(SQLITE3_INCLUDE_DIR)
	message("SQLite3           : yes")
else (SQLITE3_FOUND)
	set(SQLITE3_LIBRARIES "")
	message("SQLite3           : no")
endif (SQLITE3_FOUND)

if(CMAKE_BUILD_TYPE MATCHES Debug)
	ADD_DEFINITIONS(-O0)
	ADD_DEFINITIONS(-ggdb)
	ADD_DEFINITIONS(-DDEBUG)
	ADD_DEFINITIONS(-Wall)
	ADD_DEFINITIONS(-W)
	ADD_DEFINITIONS(-Wcast-align)
	ADD_DEFINITIONS(-Wextra -Wno-sign-compare -Wno-unused-parameter)
	ADD_DEFINITIONS(-Winit-self)
	ADD_DEFINITIONS(-Wmissing-declarations)
	ADD_DEFINITIONS(-Wpointer-arith)
#	ADD_DEFINITIONS(-Wreorder)
#	ADD_DEFINITIONS(-Woverloaded-virtual)
#	ADD_DEFINITIONS(-Wsign-promo)
	ADD_DEFINITIONS(-Wundef -Wunused)
	message("Debug             : yes")
else(CMAKE_BUILD_TYPE MATCHES Debug)
	message("Debug             : no (run \"cmake . -DCMAKE_BUILD_TYPE=Debug\")")
endif(CMAKE_BUILD_TYPE MATCHES Debug)


SET(HAMLOG_VERSION 1.0)
SET(PROJECT_VERSION 1.0)

include_directories(${Boost_INCLUDE_DIRS})

ADD_SUBDIRECTORY(ui)
ADD_SUBDIRECTORY(library)
ADD_SUBDIRECTORY(server)

if(DOXYGEN_FOUND)
	message("Docs              : yes")
	ADD_SUBDIRECTORY(docs)
else(DOXYGEN_FOUND)
	message("Docs              : no (install doxygen)")
endif(DOXYGEN_FOUND)

message("----------------------")
