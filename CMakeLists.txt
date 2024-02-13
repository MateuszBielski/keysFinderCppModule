cmake_minimum_required(VERSION 3.16)

project(keysFinderCppModule)

if (MSVC)
set(wxWidgets_ROOT_DIR D:\\programy\\keysFinder)
set(wxWidgets_CONFIGURATION mswud)
endif (MSVC)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
#find_package(wxWidgets REQUIRED gl core base)
#include(${wxWidgets_USE_FILE})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

#

if (MSVC)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
	#add_compile_options(/W0)
    # add_compile_options(/showIncludes)
endif (MSVC)
if(UNIX)
    #add_compile_options(-H)
    add_compile_options(-fPIC)#static Glad is linked to dynamic Oglstuff
endif(UNIX)

#following source is html file so I resigned
#include(FetchContent) #loading submodule of cmake
#FetchContent_Declare(iniParser
    #URL https://ssarcandy.tw/ini-cpp/ini_8h_source.html
    #DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../download
    #SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include
#)
#FetchContent_MakeAvailable(iniParser)
set(includes 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set ( CXX_SRCS
    src/main.cpp
    src/edgeprocessing.cpp
)
#add_compile_definitions(_USE_MATH_DEFINES)#for using M_PI constant
add_executable(${CMAKE_PROJECT_NAME} ${CXX_SRCS})
#add_subdirectory(lib/oglstuff)
#add_subdirectory(lib/wxApp)
target_link_libraries( ${CMAKE_PROJECT_NAME} ${OpenCV_LIBS} )
#target_link_libraries(${CMAKE_PROJECT_NAME} 
#    PUBLIC 
#    ${wxWidgets_LIBRARIES}
#   OglStuff
#    wxApp
#	)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${includes})
#target_include_directories(wxApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)


            

    
