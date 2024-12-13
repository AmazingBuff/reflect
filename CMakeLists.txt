cmake_minimum_required(VERSION 3.25)

project(
	reflect
	LANGUAGES CXX
	DESCRIPTION "A simple reflection library for C++"
)

file(GLOB_RECURSE HEADER_FILES include/*.h)
file(GLOB_RECURSE SOURCE_FILES include/*.cpp)

add_executable(
	${PROJECT_NAME}
	${HEADER_FILES}
	src/main.cpp
)

target_compile_features(
	${PROJECT_NAME}
	PUBLIC
	cxx_std_23
)

target_compile_options(
	${PROJECT_NAME}
	PUBLIC
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
	$<$<CXX_COMPILER_ID:GNU>:-Wextra -Wall -pedantic>
	$<$<CXX_COMPILER_ID:Clang>:-Wc++23-extensions>
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)