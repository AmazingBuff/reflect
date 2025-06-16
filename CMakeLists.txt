cmake_minimum_required(VERSION 3.25)

project(
	reflect
	LANGUAGES CXX
	DESCRIPTION "A simple reflection library for C++"
)

add_library(
	${PROJECT_NAME}
	INTERFACE
)

target_compile_features(
	${PROJECT_NAME}
	INTERFACE
	cxx_std_23
)

target_compile_options(
	${PROJECT_NAME}
	INTERFACE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
	$<$<CXX_COMPILER_ID:GNU>:-Wextra -Wall -pedantic>
	$<$<CXX_COMPILER_ID:Clang>:-Wc++23-extensions>
)

target_include_directories(
	${PROJECT_NAME}
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)


file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
find_package(Clang)

add_executable(
	${PROJECT_NAME}_exec
	${SOURCE_FILES}
)

target_link_libraries(
	${PROJECT_NAME}_exec
	PRIVATE
	clangFrontend
	clangTooling
	${PROJECT_NAME}
)

target_include_directories(
	${PROJECT_NAME}_exec
	PRIVATE
	${CLANG_INCLUDE_DIRS}
)


set(REFLECT_OUTPUT_DIR ${CMAKE_BINARY_DIR}/reflect)

get_property(targets DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
foreach(target ${targets})
	get_target_property(target_sources ${target} SOURCES)
	if("${target_sources}" STREQUAL "target_sources-NOTFOUND" OR "${target_sources}" STREQUAL "${PROJECT_NAME}_exec")
		continue()
	endif ()

	foreach (source ${target_sources})
		get_filename_component(source_path ${source} ABSOLUTE)
		list(APPEND COMMAND_ARGS "-i" ${source_path})
	endforeach ()

	add_custom_target(reflect_${target}
			COMMAND ${PROJECT_NAME}_exec ${COMMAND_ARGS} -O ${REFLECT_OUTPUT_DIR}
			DEPENDS ${PROJECT_NAME}_exec
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)
endforeach()