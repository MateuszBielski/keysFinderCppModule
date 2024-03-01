cmake_minimum_required(VERSION 3.16)

project(keysFinderCppModule)

if (MSVC)
set(wxWidgets_ROOT_DIR D:\\programy\\keysFinder)
set(wxWidgets_CONFIGURATION mswud)
endif (MSVC)
#find_package( OpenCV REQUIRED )


#find_package(wxWidgets REQUIRED gl core base)
#include(${wxWidgets_USE_FILE})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)


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


set(opencvExternDir ${CMAKE_CURRENT_SOURCE_DIR}/../opencv)
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
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${opencvExternDir}/opencv-build)
message("execute build")
# execute_process(COMMAND ${CMAKE_COMMAND} --build 	${opencvExternDir}/opencv-build
		# --target install --config debug
		# WORKING_DIRECTORY ${opencvExternDir}/opencv-install)
execute_process(COMMAND ${CMAKE_COMMAND} --install ${opencvExternDir}/opencv-build --config Debug
	WORKING_DIRECTORY ${opencvExternDir}/opencv-install)

message("execute installation")
#zainstalowane moduły na pracowym: calib3d core dnn features2d flann gapi highgui imgcodecs imgproc ml objdetect photo stitching ts video videoio

set(OpenCV_DIR ${opencvExternDir}/opencv-install)
find_package(OpenCV REQUIRED)
# include_directories(${OpenCV_INCLUDE_DIRS})

set(includes 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
	${opencvExternDir}/opencv-install/include
)

set ( CXX_SRCS
    src/main.cpp
    src/edgeprocessing.cpp
)

#add_compile_definitions(_USE_MATH_DEFINES)#for using M_PI constant
add_executable(${CMAKE_PROJECT_NAME} ${CXX_SRCS})


target_link_libraries( ${CMAKE_PROJECT_NAME} 
	# ${opencvExternDir}/opencv-install/x64/vc17/lib
	${OpenCV_LIBS}
)
#target_link_libraries(${CMAKE_PROJECT_NAME} 
#    PUBLIC 
#    ${wxWidgets_LIBRARIES}
#   OglStuff
#    wxApp
#	)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS} ${includes})
#target_include_directories(wxApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)


            

    
