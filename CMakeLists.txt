project(dnsp2p C)
cmake_minimum_required(VERSION 2.4)

if(CMAKE_BINARY_DIR STREQUAL ${CMAKE_SOURCE_DIR})
        message( FATAL_ERROR "Building stuff in source dir sucks. Remove CMakeCache.txt from ${CMAKE_SOURCE_DIR} and read the README.Build" )
endif(CMAKE_BINARY_DIR STREQUAL ${CMAKE_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(Tokyocabinet REQUIRED)
find_package(Libevent2 REQUIRED)

add_definitions(-Wall -Wextra -std=gnu99 -Werror)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(dht)
add_subdirectory(seccure)
add_subdirectory(libbenc)

include_directories(${LIBEVENT2_INCLUDE_DIRS} ${TOKYOCABINET_INCLUDE_DIRS})
add_library(ffff ffff.c properties.c admin.c dns.c rbtree.c ops.c op_get.c crypto.c)
target_link_libraries(ffff seccure dht ${LIBEVENT2_LIBRARIES} ${TOKYOCABINET_LIBRARIES})

add_executable(dnsp2p stub.c)
target_link_libraries(dnsp2p ffff)
install(TARGETS dnsp2p RUNTIME DESTINATION sbin)
