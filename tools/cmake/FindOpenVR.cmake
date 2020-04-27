# Find the OpenVR SDK
# NOTE: there is no default installation path as the code needs to be build
# This module defines:
# OPENVR_FOUND, if false do not try to link against the Open VR SDK
# OPENVR_LIBRARIES, the name of the Open VR SDK libraries to link against
# OPENVR_INCLUDE_DIR, the Open VR SDK include directory
#
# You can also specify the environment variable OPENVR_DIR or define it with
# -DOPENVR_DIR=... to hint at the module where to search for the Open VR SDK if it's
# installed in a non-standard location.

IF (DEFINED ENV{OPENVR_DIR})
    SET(OPENVR_DIR "$ENV{OPENVR_DIR}")
ENDIF()

SET(OPENVR_DIR
    "${OPENVR_DIR}"
    CACHE
    PATH
    "Root directory to search for OpenVR SDK")

FIND_PATH(OPENVR_INCLUDE_DIR openvr.h
	HINTS
	${OPENVR_DIR}
	PATH_SUFFIXES headers/
	# TODO: Unsure on handling of the possible default install locations
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local/include/
	/usr/include/
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
)

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	IF(UNIX OR MINGW)
		set(LIB_PATH_SUFFIX "lib/linux64/")
	ELSEIF(MSVC)
		set(LIB_PATH_SUFFIX "lib/win64/")
	ELSE()
		message(ERROR "Error: Unsupported 64 bit configuration")
	ENDIF()
ELSE()
	IF(UNIX OR MINGW)
		set(LIB_PATH_SUFFIX "lib/linux32/")
	ELSEIF(MSVC)
		set(LIB_PATH_SUFFIX "lib/win32/")
	ELSEIF(APPLE)
		set(LIB_PATH_SUFFIX "lib/osx32/")
	ELSE()
		message(ERROR "Error: Unsupported 32 bit configuration")
	ENDIF()
ENDIF()

# This hack is inspired by FindBoost.cmake. It ensures that only shared objects are found. Linking a SO with a static lib is not possible 
# in Linux. On other systems, this should be no problem.
SET( _ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
IF( CMAKE_HOST_SYSTEM MATCHES "Linux" )
    SET( CMAKE_FIND_LIBRARY_SUFFIXES .so )
ENDIF()

FIND_LIBRARY(OPENVR_LIBRARY NAMES openvr_api openvr_api.lib libopenvr_api libopenvr_api.so
	HINTS
	${OPENVR_DIR}
	PATH_SUFFIXES ${LIB_PATH_SUFFIX}
	# TODO: I don't know if these will be correct if people have installed
	# the library on to their system instead of just using the git repo or w/e
	PATHS
	/sw
	/opt/local
	/opt/csw
	/opt
)

SET( CMAKE_FIND_LIBRARY_SUFFIXES ${_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES} )

SET(OPENVR_FOUND FALSE)
IF(OPENVR_LIBRARY AND OPENVR_INCLUDE_DIR)
	SET(OPENVR_LIBRARY ${OPENVR_LIBRARY} CACHE STRING "Which OpenVR library to link against")
	# No debug library for OpenVR
	SET(OPENVR_LIBRARY_DEBUG ${OPENVR_LIBRARY})
	SET(OPENVR_LIBRARIES optimized ${OPENVR_LIBRARY} debug ${OPENVR_LIBRARY_DEBUG})
	SET(OPENVR_FOUND TRUE)
ENDIF()

IF( OPENVR_FOUND )
   IF( NOT openvr_FIND_QUIETLY )
       MESSAGE( STATUS "Found openvr: ${OPENVR_LIBRARY} and include in ${OPENVR_INCLUDE_DIR}" )
   ENDIF()
ELSE()
   IF( openvr_FIND_REQUIRED )
       MESSAGE( FATAL_ERROR "Could not find openvr. You can specify OPENVR_DIR environment variable to help OpenWalnut finding it." )
   ENDIF()
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenVRSDK REQUIRED_VARS OPENVR_LIBRARIES OPENVR_INCLUDE_DIR)

MARK_AS_ADVANCED(OPENVR_LIBRARIES OPENVR_INCLUDE_DIR)