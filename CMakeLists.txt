cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

# add options for testing
option(ENABLE_COVERAGE "Enable code coverage testing." OFF)
option(ENABLE_MEMCHECK "Enable testing for memory leaks." OFF)
option(ENABLE_PROFILING "Enable profiling code usage." OFF)

# define project name
project (clist VERSION 0.1)

# test project name
set(PROJECT_TEST ${PROJECT_NAME}-test)

# define public headers
set(HEADERS
		clist/list-item.h
		clist/list-vtable.h
		clist/list.h
		)

# define sources
set(SOURCES 
	list-item.c 
	list-single.c
	list.c
	${HEADERS}
)

include_directories(SYSTEM ${CMAKE_CURRENT_LIST_DIR})

# define library
add_library(${PROJECT_NAME} ${SOURCES})

# install path
install(FILES ${HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${INSTALL_DIRECTORY}")

# Setup testing
enable_testing()

# test executable
add_executable(${PROJECT_TEST}
  list-test.c
)

# link library to test executable
target_link_libraries(${PROJECT_TEST} ${PROJECT_NAME} cmocka)

## other scripts:
## package definition
## code coverage
## memory leak checking

# set path to custom modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# create the package config install
include(CreatePackages)
create_packages("clist" DESCRIPTION "a linked list library in c")

if(ENABLE_COVERAGE)
	include(CodeCoverage)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}")
  setup_target_for_coverage(TARGET ${PROJECT_TEST} OUTPUT ${PROJECT_BINARY_DIR}/coverage)
endif()

include(ValgrindTest)
add_valgrind_profile_test(MEMCHECK ${ENABLE_MEMCHECK} PROFILING ${ENABLE_PROFILING} TARGET ${PROJECT_TEST})

