cmake_minimum_required(VERSION 3.15)
 
project(BQ769x0
    VERSION 0.0.1
    LANGUAGES C
)
 
file(GLOB_RECURSE BQ769X0_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)
 
add_library(${PROJECT_NAME} ${BQ769X0_SOURCES})
 
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
 
target_compile_features(${PROJECT_NAME} PRIVATE c_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 11
)
