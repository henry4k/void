CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)


PROJECT("Dungeon" CXX C)

SET(RootDir "${CMAKE_SOURCE_DIR}") # This is the root directory
SET(SourceDir "${RootDir}/Source") # Where the source tree is
SET(CMakeDir "${RootDir}/CMake") # Additional cmake tools go here
LIST(APPEND CMAKE_MODULE_PATH ${CMakeDir})

SET(CMAKE_BUILD_TYPE Debug)


OPTION(KJ_BUILD_SHARED_MODULES "When enabled the modules are built shared." ON)
IF(KJ_BUILD_SHARED_MODULES)
	SET(BUILD_SHARED_LIBS TRUE)
ENDIF()

SET(KJ_RESOURCE_PATH "${TargetDir}" CACHE PATH "Were the resource tree is.")
OPTION(KJ_BUNDLED "When enabled the binaries go with the resource tree." ON)
IF(KJ_BUNDLED)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${KJ_RESOURCE_PATH})
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${KJ_RESOURCE_PATH})
	SET(CMAKE_SHARED_LIBRARY_PREFIX "")
ENDIF()

IF(MSVC)
	# Force to always compile with W4
	IF(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	ELSE()
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	ENDIF()
ELSEIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	# Update if necessary
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpointer-arith -Wcast-qual -Winit-self -Wno-variadic-macros -Wno-long-long -Wno-sign-compare -Wno-unused-result")
ENDIF()


MACRO(RequirePackage Package)
	FIND_PACKAGE(${Package})
	STRING(TOUPPER ${Package} PackageUppercase)
	IF(NOT "${PackageUppercase}_FOUND" AND NOT "${Package}_FOUND")
		GET_FILENAME_COMPONENT(Module ${CMAKE_CURRENT_SOURCE_DIR} NAME)
		MESSAGE("${Module}: Can't find ${Package}")
		RETURN()
	ENDIF()
ENDMACRO()


ADD_SUBDIRECTORY("Source")