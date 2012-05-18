# - Try to find GLFW
# Once done this will define
#
#  GLFW_FOUND - system has GLFW
#  GLFW_INCLUDE_DIR - the include directory
#  GLFW_LIBRARY - The GLFW library


FIND_PATH( GLFW_INCLUDE_DIR GL/glfw.h $ENV{GLFWDIR}/include
                                      ~/Library/Frameworks/GLFW.framework/Headers
                                      /Library/Frameworks/GLFW.framework.Headers
                                      /usr/local/include
                                      /usr/local/X11R6/include
                                      /usr/X11R6/include
                                      /usr/X11/include
                                      /usr/include/X11
                                      /usr/include
                                      /opt/X11/include
                                      /opt/include )

FIND_LIBRARY( GLFW_LIBRARY NAMES glfw PATHS $ENV{GLFWDIR}/lib
                                              $ENV{GLFWDIR}/support/msvc80/Debug
                                              $ENV{GLFWDIR}/support/msvc80/Release
                                              /usr/local/lib
                                              /usr/local/X11R6/lib
                                              /usr/X11R6/lib
                                              /usr/X11/lib
                                              /usr/lib/X11
                                              /usr/lib
                                              /opt/X11/lib
                                              /opt/lib )

MARK_AS_ADVANCED(GLFW_INCLUDE_DIR GLFW_LIBRARY)

IF(GLFW_INCLUDE_DIR AND GLFW_LIBRARY)
	SET(GLFW_FOUND TRUE)
ELSE(GLFW_INCLUDE_DIR AND GLFW_LIBRARY)
	SET(GLFW_FOUND FALSE)
ENDIF(GLFW_INCLUDE_DIR AND GLFW_LIBRARY)