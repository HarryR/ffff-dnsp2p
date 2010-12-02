#include "admin.h"

#include <assert.h>
#include <string.h>

// TODO: go implement a basic REST interface for managing the service

f4admin_ctx_t *
f4admin_new( f4_ctx_t *f4_ctx ) {
    assert( f4_ctx != NULL );

    f4admin_ctx_t *ctx = (f4admin_ctx_t *)calloc(sizeof(f4admin_ctx_t),1);
    ctx->f4 = f4_ctx;
    return ctx;
}

bool
f4admin_init( f4admin_ctx_t *ctx ) {
    assert( ctx != NULL );

    ctx->http = evhttp_new(ctx->f4->base);
    // derpy derpy
    // Go setup the evhttp server and setup bindings etc.
    return true;
}

void
f4admin_free(f4admin_ctx_t *ctx) {
    // dorpy dorpy
    // go destroy evhttp server
    if( ctx->http != NULL ) evhttp_free(ctx->http);
    memset(ctx, 0, sizeof(f4admin_ctx_t));
    free(ctx);
}
