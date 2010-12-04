project(dnsp2p C)
cmake_minimum_required(VERSION 2.8)

include_directories(/usr/local/include third_party/include)

find_library(LIBEVENT event)
find_library(LIBTOKYOCABINET tokyocabinet)

add_definitions(-Wall -Wextra -std=gnu99)

add_subdirectory(dht)
add_subdirectory(seccure)

add_library(ffff ffff.c properties.c admin.c dns.c rbtree.c ops.c)
target_link_libraries(ffff seccure dht ${LIBEVENT} ${LIBTOKYOCABINET})

add_executable(dnsp2p stub.c)
target_link_libraries(dnsp2p ffff)
install(TARGETS dnsp2p RUNTIME DESTINATION sbin)
