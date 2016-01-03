cmake_minimum_required(VERSION 3.0)

project( Coherent-Line-Drawing )

find_package( OpenCV REQUIRED )


FIND_PACKAGE(wxWidgets REQUIRED)
INCLUDE(${wxWidgets_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable( Coherent-Line-Drawing
"${PROJECT_SOURCE_DIR}/src/main.cpp" 
"${PROJECT_SOURCE_DIR}/src/ETF.cpp" 
"${PROJECT_SOURCE_DIR}/src/postProcessing.cpp" 
"${PROJECT_SOURCE_DIR}/src/gui.cpp"  
"${PROJECT_SOURCE_DIR}/include/ETF.h" 
"${PROJECT_SOURCE_DIR}/include/postProcessing.h" 
"${PROJECT_SOURCE_DIR}/include/gui.h" 
)

target_link_libraries( Coherent-Line-Drawing ${OpenCV_LIBS} ${wxWidgets_LIBRARIES} )