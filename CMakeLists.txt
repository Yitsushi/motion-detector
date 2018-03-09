cmake_minimum_required(VERSION 2.8)

project( MotionDetertor )
find_package( OpenCV REQUIRED )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (CMAKE_CXX_STANDARD 14)
SET(CPACK_GENERATOR "TGZ")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include(CPack)

#add_executable( MotionDetertor src/main.cpp )
#target_link_libraries( MotionDetertor ${OpenCV_LIBS} )

# Add example executable
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/core CORE)
add_executable(MotionDetertor ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${CORE})
target_link_libraries(MotionDetertor ${OpenCV_LIBS})
install(TARGETS MotionDetertor RUNTIME DESTINATION "${CMAKE_BINARY_DIR}/bin")
