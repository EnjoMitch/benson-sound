cmake_minimum_required(VERSION 3.6.2)
project( benson-sound )

find_package(SDL REQUIRED)

include_directories(${SDL_INCLUDE_DIRS})

file( GLOB SRCS src/*.cpp src/*.h)

add_executable(${PROJECT_NAME} ${SRCS} ${ASM_OBJECTS})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS_NO_SHARED true)
target_link_libraries(${PROJECT_NAME} SDL)

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


