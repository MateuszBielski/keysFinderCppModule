cmake_minimum_required(VERSION 3.16)

project(keysFinderCppModule)


#find_package( OpenCV REQUIRED )


#find_package(wxWidgets REQUIRED gl core base)
#include(${wxWidgets_USE_FILE})

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)


if (MSVC)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
	# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") #MSVCRTD.lib(exe_winmain.obj) : error LNK2019
	set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
    set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
	#add_compile_options(/W0)
    # add_compile_options(/showIncludes)
endif (MSVC)
if(UNIX)
    #add_compile_options(-H)
    add_compile_options(-fPIC)#static Glad is linked to dynamic Oglstuff
endif(UNIX)


set(opencvExternDir ${CMAKE_CURRENT_SOURCE_DIR}/../opencv)
set(opencvInstallDir ${opencvExternDir}/opencv-install)

if (MSVC)
set(OPENCV_ROOT_DIR ${opencvInstallDir})
# set(wxWidgets_CONFIGURATION mswud)
endif (MSVC)

# find_package(OpenCV)
# message("find_package(OpenCV REQUIRED)")

# if(NOT ${OPENCV_FOUND})


	find_package(OpenCV REQUIRED PATHS ${opencvInstallDir})
	message("find_package\(OpenCV REQUIRED PATHS \$\{opencvInstallDir\}\)")
# endif()

if(NOT ${OPENCV_FOUND})
	find_package(OpenCV REQUIRED PATHS ${opencvInstallDir})
	message("configure_file(CMakeLists.txt.in....")

configure_file(CMakeLists.txt.in
        ${opencvExternDir}/opencv-configuration/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY ${opencvExternDir}/opencv-configuration)
execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${opencvExternDir}/opencv-configuration)
message("execute download")

execute_process(COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} 
		-D CMAKE_BUILD_TYPE=DEBUG 
		-DCMAKE_INSTALL_PREFIX=${opencvExternDir}/opencv-install 
		-DOPENCV_GENERATE_PKGCONFIG=ON -DBUILD_EXAMPLES=OFF -DINSTALL_PYTHON_EXAMPLES=OFF -DINSTALL_C_EXAMPLES=OFF -DBUILD_JAVA=OFF -DWITH_TESSERACT=ON 
		${opencvExternDir}/opencv-src
		WORKING_DIRECTORY ${opencvExternDir}/opencv-build
		)
message("execute configuration")


execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Debug
        WORKING_DIRECTORY ${opencvExternDir}/opencv-build)
message("execute build Debug")

execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release
        WORKING_DIRECTORY ${opencvExternDir}/opencv-build)
message("execute build Release")

execute_process(COMMAND ${CMAKE_COMMAND} --install . --config Debug --prefix ${opencvInstallDir}
	WORKING_DIRECTORY ${opencvExternDir}/opencv-build)
message("execute installation Debug")

execute_process(COMMAND ${CMAKE_COMMAND} --install . --config Release --prefix ${opencvInstallDir}
	WORKING_DIRECTORY ${opencvExternDir}/opencv-build)
message("execute installation Release")
# find_package(OpenCV REQUIRED PATHS D:\\programowanie\\opencvStandalone\\install)
find_package(OpenCV REQUIRED PATHS ${opencvInstallDir})
endif()


set(includes 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
	# ${opencvInstallDir}/include
	${OpenCV_INCLUDE_DIRS}
)

set ( CXX_SRCS
    src/main.cpp
    src/edgeprocessing.cpp
    src/timemeasure.cpp
)

#add_compile_definitions(_USE_MATH_DEFINES)#for using M_PI constant
add_executable(${CMAKE_PROJECT_NAME} ${CXX_SRCS})


target_link_libraries( ${CMAKE_PROJECT_NAME} 
	${OpenCV_LIBS}
)
# add_library(${OpenCV_LIBS}
# SHARED ${CMAKE_PROJECT_NAME}
# )

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${includes})

if (MSVC)
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=${_OpenCV_LIB_PATH};")
endif (MSVC)

            

    
