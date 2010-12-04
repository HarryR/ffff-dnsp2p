#ifndef ADMIN_H_
#define ADMIN_H_

#include "ffff.h"
#include <event2/http.h>

struct f4admin_ctx {
    f4_ctx_t *f4;
    struct evhttp *http;
};
typedef struct f4admin_ctx f4admin_ctx_t;

f4admin_ctx_t *f4admin_new( f4_ctx_t *f4_ctx );
bool f4admin_init( f4admin_ctx_t *ctx );
void f4admin_free(f4admin_ctx_t *ctx);

#endif
