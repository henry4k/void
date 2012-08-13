FILE(GLOB Sources "*.?pp")
LIST(REMOVE_ITEM Sources "Main.cpp")
ADD_LIBRARY(VoidServer ${Sources})
TARGET_LINK_LIBRARIES(VoidServer VoidCommon)

ADD_EXECUTABLE(voidd "Main.cpp")
TARGET_LINK_LIBRARIES(voidd VoidServer)