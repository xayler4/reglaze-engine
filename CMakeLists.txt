cmake_minimum_required(VERSION 3.15...3.30)

project(
	reglaze-engine 
	VERSION 1.0)

set(
	INCLUDE_DIR
	${CMAKE_SOURCE_DIR}/include/reglaze)

set(
	SRC_DIR
	${CMAKE_SOURCE_DIR}/src)

set(
	INCLUDE
	${INCLUDE_DIR}/base.h
	${INCLUDE_DIR}/reglaze.h
	${INCLUDE_DIR}/core/application.h
	${INCLUDE_DIR}/core/engine.h)

set(
	SRC
	${SRC_DIR}/memory/memory_manager.h
	${SRC_DIR}/memory/memory_block.h
	${SRC_DIR}/memory/stack_allocator.h
	${SRC_DIR}/core/entrypoint.cpp
	${SRC_DIR}/core/application.cpp
	${SRC_DIR}/core/engine.cpp
	${SRC_DIR}/memory/memory_manager.cpp
	${SRC_DIR}/memory/memory_block.cpp)

set(
	TESTBED_SRC
	${CMAKE_SOURCE_DIR}/testbed/app.cpp)

add_library(
	${PROJECT_NAME}
	STATIC
	${INCLUDE}
	${SRC})

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${INCLUDE_DIR}
	${SRC_DIR})

target_compile_features(
	${PROJECT_NAME} 
	PUBLIC 
	cxx_std_20)

set_target_properties(
	${PROJECT_NAME} 
	PROPERTIES 
	CXX_EXTENSIONS OFF
	LINKER_LANGUAGE CXX)

add_executable(
	testbed 
	${TESTBED_SRC})

target_link_libraries(
	testbed
	PRIVATE
	${PROJECT_NAME})

target_include_directories(
	testbed
	PRIVATE
	${INCLUDE_DIR})
