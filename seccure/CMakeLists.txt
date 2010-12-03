find_library(LIBGCRYPT gcrypt)

set(SECCURE_SOURCES
    aes256ctr.c
    curves.c
    ecc.c
    numtheory.c
    protocol.c
    seccure.c
    serialize.c
    )

add_library(seccure ${SECCURE_SOURCES})
target_link_libraries(seccure ${LIBGCRYPT})
