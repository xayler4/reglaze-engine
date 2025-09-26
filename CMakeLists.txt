cmake_minimum_required(VERSION 3.15...3.30)

project(
	reglaze-engine 
	VERSION 1.0
)

find_package(easer-lib 1.0 REQUIRED)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

set(
	INCLUDE_DIR
	${CMAKE_SOURCE_DIR}/include/reglaze)

set(
	SRC_DIR
	${CMAKE_SOURCE_DIR}/src)

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
	${INCLUDE_DIR}/core/preferences.h
	${INCLUDE_DIR}/containers/fixed_string.h
)

set(
	SRC
	${SRC_DIR}/memory/memory_manager.h
	${SRC_DIR}/memory/memory_block.h
	${SRC_DIR}/memory/stack_allocator.h
	${SRC_DIR}/errors.cpp
	${SRC_DIR}/core/entrypoint.cpp
	${SRC_DIR}/core/logger.cpp
	${SRC_DIR}/core/application.cpp
	${SRC_DIR}/core/engine.cpp
	${SRC_DIR}/core/preferences.cpp
	${SRC_DIR}/containers/to_hash.cpp
	${SRC_DIR}/memory/memory_manager.cpp
	${SRC_DIR}/memory/memory_block.cpp
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

target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
	${SRC_DIR}/pch.h
)

target_link_libraries(
	${PROJECT_NAME}
	easer-lib
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

