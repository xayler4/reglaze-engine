cmake_minimum_required(VERSION 3.15...3.30)

project(
	reglaze-engine 
	VERSION 1.0
)

find_package(Boost REQUIRED)
find_package(easer-lib 1.0 REQUIRED)
find_package(SDL3 REQUIRED CONFIG REQUIRED)


option(ENABLE_ASAN "Enable Address Sanitizer" OFF)

if(ENABLE_ASAN)
    # Apply to all targets defined after this point
    add_compile_options(-fsanitize=address -g -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

set(
	INCLUDE_DIR
	${CMAKE_SOURCE_DIR}/include/reglaze)

set(
	SRC_DIR
	${CMAKE_SOURCE_DIR}/src)

set(
	SHADERS_DIR
	${SRC_DIR}/gfx/shaders)

set(
	COMPILED_SHADERS_DIR
	${SRC_DIR}/gfx/compiled_shaders)

file(MAKE_DIRECTORY ${COMPILED_SHADERS_DIR})

file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS 
	"${SHADERS_DIR}/*.vert" 
    "${SHADERS_DIR}/*.frag"
)
set(COMPILED_SHADER_HEADERS "")

foreach(SHADER_FILE IN LISTS SHADER_FILES)
    # 1. Get the base name of the source file (e.g., "sprite.vert")
    get_filename_component(BASE_NAME "${SHADER_FILE}" NAME)

    # 2. Define the exact output paths for this specific shader
	set(SPV_OUTPUT_PATH "${COMPILED_SHADERS_DIR}/${BASE_NAME}.spv")
	set(HEADER_OUTPUT_PATH "${COMPILED_SHADERS_DIR}/${BASE_NAME}.spv.h")

    # 3. Extract just the filename and directory for xxd
    get_filename_component(SPV_FILENAME "${SPV_OUTPUT_PATH}" NAME)
    get_filename_component(SPV_DIR "${SPV_OUTPUT_PATH}" DIRECTORY)

    # 4. Generate the build commands
    add_custom_command(
        OUTPUT "${HEADER_OUTPUT_PATH}"
        
        # Compile GLSL to SPIR-V 
        COMMAND glslc "${SHADER_FILE}" -o "${SPV_OUTPUT_PATH}"
        
        # Run xxd using ONLY the filename
        COMMAND xxd -i "${SPV_FILENAME}" > "${HEADER_OUTPUT_PATH}"
        
        # Force xxd to run inside the output folder so the C array name is clean
        WORKING_DIRECTORY "${SPV_DIR}"
        
        DEPENDS "${SHADER_FILE}"
    )
    
    # Append the generated header to our tracking list
    list(APPEND COMPILED_SHADER_HEADERS "${HEADER_OUTPUT_PATH}")
endforeach()

add_custom_target(
	CompileShaders
	DEPENDS
	${COMPILED_SHADER_HEADERS}
)

set(
	INCLUDE
	${INCLUDE_DIR}/base.h
	${INCLUDE_DIR}/config.h
	${INCLUDE_DIR}/config.def.h
	${INCLUDE_DIR}/errors.h
	${INCLUDE_DIR}/reglaze.h
	${INCLUDE_DIR}/core/application.h
	${INCLUDE_DIR}/core/engine.h
	${INCLUDE_DIR}/core/logger.h
	${INCLUDE_DIR}/core/logger.tpp
	${INCLUDE_DIR}/core/attribute.h
	${INCLUDE_DIR}/core/preferences.h
	${INCLUDE_DIR}/core/scene.h
	${INCLUDE_DIR}/core/ecs.h
	${INCLUDE_DIR}/core/asset_manager.h
	${INCLUDE_DIR}/core/action_manager.h
	${INCLUDE_DIR}/containers/fixed_string.h
	${INCLUDE_DIR}/memory/stack_allocator.h
	${INCLUDE_DIR}/memory/free_list_allocator.h
	${INCLUDE_DIR}/components/transform_component.h
	${INCLUDE_DIR}/components/sprite_component.h
	${INCLUDE_DIR}/systems/sprite_extractor_system.h
	${INCLUDE_DIR}/systems/camera_extractor_system.h
	${INCLUDE_DIR}/gfx/context.h
	${INCLUDE_DIR}/gfx/renderer.h
	${INCLUDE_DIR}/gfx/renderer_commands.h
	${INCLUDE_DIR}/gfx/sdl_context.h
	${INCLUDE_DIR}/gfx/sdl_renderer.h
	${INCLUDE_DIR}/math/vec.h
	${INCLUDE_DIR}/math/mat.h
	${INCLUDE_DIR}/platform/platform.h
)

set(
	SRC
	${SRC_DIR}/errors.cpp
	${SRC_DIR}/core/entrypoint.cpp
	${SRC_DIR}/core/logger.cpp
	${SRC_DIR}/core/application.cpp
	${SRC_DIR}/core/engine.cpp
	${SRC_DIR}/core/attribute.cpp
	${SRC_DIR}/core/preferences.cpp
	${SRC_DIR}/core/scene.cpp
	${SRC_DIR}/core/ecs.cpp
	${SRC_DIR}/core/asset_manager.cpp
	${SRC_DIR}/core/action_manager.cpp
	${SRC_DIR}/containers/to_hash.cpp

	${SRC_DIR}/memory/memory_manager.h
	${SRC_DIR}/memory/memory_block.h

	${SRC_DIR}/memory/stack_allocator.cpp
	${SRC_DIR}/memory/free_list_allocator.cpp
	${SRC_DIR}/memory/memory_manager.cpp
	${SRC_DIR}/memory/memory_block.cpp

	${SRC_DIR}/systems/sprite_extractor_system.cpp
	${SRC_DIR}/systems/camera_extractor_system.cpp

	${SRC_DIR}/gfx/renderer.cpp
	${SRC_DIR}/gfx/sdl_context.cpp
	${SRC_DIR}/gfx/sdl_renderer.cpp
	${SRC_DIR}/platform/platform.cpp

	${SRC_DIR}/external/stb_image.h
	${SRC_DIR}/external/stb_image.cpp
)

set(
	TESTBED_SRC
	${CMAKE_SOURCE_DIR}/testbed/app.cpp
)

set(
	CMAKE_EXPORT_COMPILE_COMMANDS ON
)

# if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
#     find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
#     set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=*")
# endif()

add_library(
	${PROJECT_NAME}
	STATIC
	${INCLUDE}
	${SRC}
)

add_dependencies(${PROJECT_NAME} CompileShaders)

target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
	${SRC_DIR}/pch.h
)

target_link_libraries(
	${PROJECT_NAME}
	easer-lib
	SDL3::SDL3
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${INCLUDE_DIR}
	${SRC_DIR}
)

target_compile_features(
	${PROJECT_NAME} 
	PUBLIC 
	cxx_std_20
)

set_target_properties(
	${PROJECT_NAME} 
	PROPERTIES 
	CXX_EXTENSIONS OFF
	LINKER_LANGUAGE CXX
)

add_executable(
	testbed 
	${TESTBED_SRC}
)

target_link_libraries(
	testbed
	PRIVATE
	${PROJECT_NAME}
)

target_include_directories(
	testbed
	PRIVATE
	${INCLUDE_DIR}
)

