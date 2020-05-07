CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(Coherent-Line-Drawing)

FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(wxWidgets REQUIRED)
FIND_PACKAGE(Boost REQUIRED)

INCLUDE(${wxWidgets_USE_FILE})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

SET(DEFAULT_BUILD_TYPE "Release")
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

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

SET(LINK_LIBS "boost_program_options;${OpenCV_LIBS}")
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    IF(CMAKE_BUILD_TYPE MATCHES Debug)
        MESSAGE("Debug mode, enable gcov")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
        SET(LINK_LIBS "gcov;${LINK_LIBS}")
    ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)
ENDIF()

TARGET_LINK_LIBRARIES(cld "${LINK_LIBS}")
