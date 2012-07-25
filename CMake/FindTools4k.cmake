# - Try to find tools4k
# Once done this will define
#
#  Tools4k_FOUND - system has Tools4k
#  Tools4k_INCLUDE_DIR - the include directory
#  Tools4k_LIBRARY - The Tools4k library

FIND_PATH(Tools4k_INCLUDE_DIR tools4k/tools4k.h)
FIND_LIBRARY(Tools4k_LIBRARY NAMES tools4k)

MARK_AS_ADVANCED(Tools4k_INCLUDE_DIR Tools4k_LIBRARY)

IF(Tools4k_INCLUDE_DIR AND Tools4k_LIBRARY)
	SET(Tools4k_FOUND TRUE)
ELSE()
	SET(Tools4k_FOUND FALSE)
ENDIF()


IF(Tools4k_FOUND AND UNIX AND NOT APPLE)
	INCLUDE(CheckFunctionExists)

	CHECK_FUNCTION_EXISTS("clock_gettime" HAVE_CLOCK_GETTIME)
	IF(NOT HAVE_CLOCK_GETTIME)
		SET(CMAKE_REQUIRED_LIBRARIES "rt")
		CHECK_FUNCTION_EXISTS("clock_gettime" HAVE_CLOCK_GETTIME_RT)
		UNSET(CMAKE_REQUIRED_LIBRARIES)
		IF(HAVE_CLOCK_GETTIME_RT)
			LIST(APPEND Tools4k_LIBRARY "rt")
		ELSE()
			MESSAGE(WARNING "Can't find clock_gettime! Please report this case the developers!")
		ENDIF()
	ENDIF()
ENDIF()