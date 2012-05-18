FILE(GLOB Sources "*.?pp")
ADD_EXECUTABLE(kjd ${Sources})
TARGET_LINK_LIBRARIES(kjd KjCommon)