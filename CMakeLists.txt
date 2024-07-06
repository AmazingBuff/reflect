cmake_minimum_required(VERSION 3.25)

project(
	reflect
	LANGUAGES CXX
	DESCRIPTION "A simple reflection library for C++"
)

file(GLOB_RECURSE HEADER_FILES include/*.h)
file(GLOB_RECURSE SOURCE_FILES include/*.cpp)

add_library(
	${PROJECT_NAME}
	STATIC
	${HEADER_FILES}
	${SOURCE_FILES}
)

target_compile_features(
	${PROJECT_NAME}
	INTERFACE
	cxx_std_23
)

target_include_directories(
	${PROJECT_NAME}
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

target_compile_options(
	${PROJECT_NAME}
	INTERFACE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
	$<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
	$<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -pedantic>
)

add_executable(
	${PROJECT_NAME}_example
	src/main.cpp
)

target_link_libraries(
	${PROJECT_NAME}_example
	PRIVATE
	${PROJECT_NAME}
)

target_include_directories(
	${PROJECT_NAME}_example
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(
	${PROJECT_NAME}_example
	PRIVATE
	cxx_std_23
)