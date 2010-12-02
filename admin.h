#ifndef ADMIN_H_
#define ADMIN_H_

// Standard logging method
#define LOG(...) fprintf (stderr, __VA_ARGS__)

#include "ffff.h"

struct f4admin_ctx {
    f4_ctx_t *f4;
    struct evhttp *http;
};
typedef struct f4admin_ctx f4admin_ctx_t;

f4admin_ctx_t *f4admin_new( f4_ctx_t *f4_ctx );
bool f4admin_init( f4admin_ctx_t *ctx );
void f4admin_free(f4admin_ctx_t *ctx);

#endif
