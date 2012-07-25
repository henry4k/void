INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/Catch/include")

MACRO(AddTest Name)
	ADD_EXECUTABLE("test${Name}" "${Name}.cpp")
	TARGET_LINK_LIBRARIES("test${Name}" ${ARGN})
# 	SET_PROPERTY(TARGET "test${Name}" PROPERTY COMPILE_DEFINITIONS "PX_TEST_NAME" "${Name}")
ENDMACRO()

AddTest(Resource KjCommon)
AddTest(Map KjCommon)
