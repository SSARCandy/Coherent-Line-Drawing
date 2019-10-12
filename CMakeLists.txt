CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(Coherent-Line-Drawing)

FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(wxWidgets REQUIRED)
FIND_PACKAGE(Boost REQUIRED)

INCLUDE(${wxWidgets_USE_FILE})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)


SET(default_build_type "Release")
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED on)


ADD_EXECUTABLE(Coherent-Line-Drawing
    "${PROJECT_SOURCE_DIR}/src/main.cpp" 
    "${PROJECT_SOURCE_DIR}/src/ETF.cpp" 
    "${PROJECT_SOURCE_DIR}/src/CLD.cpp" 
    "${PROJECT_SOURCE_DIR}/src/postProcessing.cpp" 
    "${PROJECT_SOURCE_DIR}/src/gui.cpp"  
    "${PROJECT_SOURCE_DIR}/include/ETF.h" 
    "${PROJECT_SOURCE_DIR}/include/CLD.h" 
    "${PROJECT_SOURCE_DIR}/include/postProcessing.h" 
    "${PROJECT_SOURCE_DIR}/include/gui.h" 
)
TARGET_LINK_LIBRARIES(Coherent-Line-Drawing ${OpenCV_LIBS} ${wxWidgets_LIBRARIES})

ADD_EXECUTABLE(cld
    "${PROJECT_SOURCE_DIR}/src/cmd.cpp" 
    "${PROJECT_SOURCE_DIR}/src/ETF.cpp" 
    "${PROJECT_SOURCE_DIR}/src/CLD.cpp" 
    "${PROJECT_SOURCE_DIR}/src/postProcessing.cpp" 
    "${PROJECT_SOURCE_DIR}/include/ETF.h" 
    "${PROJECT_SOURCE_DIR}/include/CLD.h" 
    "${PROJECT_SOURCE_DIR}/include/postProcessing.h" 
)
TARGET_LINK_LIBRARIES(cld
    boost_program_options
    ${OpenCV_LIBS}
)
