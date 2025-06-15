# Function to add reflection support to a target
function(add_reflection_support TARGET)
    # Parse additional arguments for source files
    cmake_parse_arguments(REFLECT "" "" "SOURCES" ${ARGN})

    if(NOT REFLECT_SOURCES)
        # If no specific sources provided, use all the target's sources
        get_target_property(REFLECT_SOURCES ${TARGET} SOURCES)
    endif()

    # Create a custom target for meta info generation
    set(META_FILES)
    foreach(SOURCE ${REFLECT_SOURCES})
        # Get absolute path of the source file
        if(NOT IS_ABSOLUTE ${SOURCE})
            get_target_property(SOURCE_DIR ${TARGET} SOURCE_DIR)
            set(SOURCE ${SOURCE_DIR}/${SOURCE})
        endif()

        # Generate output meta file path
        get_filename_component(META_FILE ${SOURCE} NAME_WE)
        set(META_FILE "${CMAKE_CURRENT_BINARY_DIR}/${META_FILE}.meta")
        list(APPEND META_FILES ${META_FILE})

        # Add custom command to generate meta info
        add_custom_command(
            OUTPUT ${META_FILE}
            COMMAND $<TARGET_FILE:reflect> ${SOURCE} -o ${META_FILE}
            DEPENDS ${SOURCE} reflect
            COMMENT "Generating meta info for ${SOURCE}"
        )
    endforeach()

    # Create meta info target
    add_custom_target(${TARGET}_meta_info
        DEPENDS ${META_FILES}
        COMMENT "Generating meta info for ${TARGET}"
    )

    # Make the original target depend on meta info generation
    add_dependencies(${TARGET} ${TARGET}_meta_info)

    # Set compile definitions for the target
    target_compile_definitions(${TARGET} PRIVATE
        REFLECT_META_DIR="${CMAKE_CURRENT_BINARY_DIR}"
    )
endfunction()

