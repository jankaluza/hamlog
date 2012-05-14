# HAMLIB_FOUND - True if TinyXML found.
# HAMLIB_INCLUDE_DIR - where to find rig.h, etc.
# HAMLIB_LIBRARIES - List of libraries when using TinyXML.
#

IF( HAMLIB_INCLUDE_DIR )
# Already in cache, be silent
SET( TinyXML_FIND_QUIETLY TRUE )
ENDIF( HAMLIB_INCLUDE_DIR )

FIND_PATH( HAMLIB_INCLUDE_DIR "rig.h"
PATH_SUFFIXES "hamlib" )

FIND_LIBRARY( HAMLIB_LIBRARIES
NAMES "hamlib"
HINTS /usr/lib64 )

# handle the QUIETLY and REQUIRED arguments and set HAMLIB_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( "FindPackageHandleStandardArgs" )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( "hamlib" DEFAULT_MSG HAMLIB_INCLUDE_DIR HAMLIB_LIBRARIES )

MARK_AS_ADVANCED( HAMLIB_INCLUDE_DIR HAMLIB_LIBRARIES )