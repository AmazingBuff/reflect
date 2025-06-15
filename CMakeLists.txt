cmake_minimum_required(VERSION 3.25)

project(
	reflect
	LANGUAGES CXX
	DESCRIPTION "A simple reflection library for C++"
)

# Add cmake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ReflectSupport)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HEADER_FILES include/*.h)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

find_package(Clang)

add_executable(
	${PROJECT_NAME}
	${HEADER_FILES}
	${SOURCE_FILES}
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
	PRIVATE
	${CLANG_INCLUDE_DIRS}
)

target_link_libraries(
	${PROJECT_NAME}
	PUBLIC
	clangFrontend
	clangTooling
)

target_compile_definitions(
	${PROJECT_NAME}
	PUBLIC
	RES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res/"
)

add_custom_command(
	TARGET ${PROJECT_NAME}
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different 
	${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_SOURCE_DIR}/.vscode/compile_commands.json
)

# Install targets
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Install cmake modules
install(FILES
    cmake/ReflectSupport.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# Create and install config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)
