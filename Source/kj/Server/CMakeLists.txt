FILE(GLOB Sources "*.?pp")
LIST(REMOVE_ITEM Sources "Main.cpp")
ADD_LIBRARY(KjServer ${Sources})
TARGET_LINK_LIBRARIES(KjServer KjCommon)

ADD_EXECUTABLE(kjd "Main.cpp")
TARGET_LINK_LIBRARIES(kjd KjServer)