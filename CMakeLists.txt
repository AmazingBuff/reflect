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
)

target_sources(
	${PROJECT_NAME}_exec
	PRIVATE
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

target_compile_features(
	${PROJECT_NAME}_exec
	PRIVATE
	cxx_std_23
)


set(REFLECT_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated CACHE STRING "")
set(REFLECT_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_exec${CMAKE_EXECUTABLE_SUFFIX} CACHE STRING "")

function(All_INCLUDE_DIRS target target_includes)
	get_target_property(real_target ${target} ALIASED_TARGET)
	if (real_target)
		set(target ${real_target})
	endif ()
	set(target_include_text ${CMAKE_CURRENT_BINARY_DIR}/${target}_include_dirs.txt)

	if(NOT EXISTS ${target_include_text})
		file(GENERATE OUTPUT ${target_include_text}
			 CONTENT "$<TARGET_PROPERTY:${target},INCLUDE_DIRECTORIES>\n$<TARGET_PROPERTY:${target},INTERFACE_INCLUDE_DIRECTORIES>")
	endif()

	if(EXISTS ${target_include_text})
		file(STRINGS ${target_include_text} evaluated_dirs)
		list(APPEND target_include_dirs ${evaluated_dirs})
	endif()

	list(APPEND target_include_dirs ${target_interface_include_dirs})

	get_target_property(target_linked_libs ${target} LINK_LIBRARIES)
	foreach(linked_lib ${target_linked_libs})
		if(TARGET ${linked_lib})
			All_INCLUDE_DIRS(${linked_lib} lib_includes)
			list(APPEND target_include_dirs ${lib_includes})
		endif()
	endforeach()

	list(REMOVE_DUPLICATES target_include_dirs)

	set(${target_includes} ${target_include_dirs} PARENT_SCOPE)
endfunction()

function(EXECUTE_REFLECT)
	get_property(targets DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
	foreach(target ${targets})
		get_target_property(target_sources ${target} SOURCES)
		if("${target_sources}" STREQUAL "target_sources-NOTFOUND" OR ${target} STREQUAL ${REFLECT_EXECUTABLE})
			continue()
		endif ()

		foreach (source ${target_sources})
			get_filename_component(source_path ${source} ABSOLUTE)
			list(APPEND COMMAND_ARGS "-i" ${source_path})
		endforeach ()

		All_INCLUDE_DIRS(${target} target_include_dirs)
		foreach (include_dir ${target_include_dirs})
			get_filename_component(include_path ${include_dir} ABSOLUTE)
			list(APPEND COMMAND_ARGS "-include" ${include_path})
		endforeach ()

		get_target_property(pch_files ${target} PRECOMPILE_HEADERS)
		foreach (pch_file ${pch_files})
			get_filename_component(pch_path ${pch_file} ABSOLUTE)
			list(APPEND COMMAND_ARGS "-pch" ${pch_path})
		endforeach ()

		get_target_property(definitions ${target} COMPILE_DEFINITIONS)
		foreach (definition ${definitions})
			list(APPEND COMMAND_ARGS "-D" ${definition})
		endforeach ()
		# this command only valid for generating reflect information
		list(APPEND COMMAND_ARGS "-D" "REFLECT=[[refl]]")

		add_custom_target(reflect_${target}
			COMMAND ${REFLECT_EXECUTABLE} ${COMMAND_ARGS} -O ${REFLECT_OUTPUT_DIR}
			DEPENDS ${REFLECT_EXECUTABLE}
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)

		target_include_directories(
			${target}
			PUBLIC
			${REFLECT_OUTPUT_DIR}
		)

		target_compile_definitions(
			${target}
			PUBLIC
			REFL_META
			REFLECT=	# for no value definition
		)
	endforeach()
endfunction()