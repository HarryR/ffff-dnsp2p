#include "dns.h"

#include <assert.h>

f4dns_ctx_t *
f4dns_new(f4_ctx_t *f4_ctx) {
    f4dns_ctx_t *ctx = (f4dns_ctx_t *)calloc(sizeof(f4dns_ctx_t), 1);
    memset(ctx, 0, sizeof(f4dns_ctx_t));
    ctx->f4 = f4_ctx;
    return ctx;
}

bool
f4dns_init(f4dns_ctx_t *ctx) {
    ctx->dns_base = evdns_base_new( ctx->f4->base, 1 );
    assert( ctx->dns_base != NULL );

    return true;
}

void
f4dns_free(f4dns_ctx_t *ctx) {
    assert( ctx != NULL );

    if( ctx->dns_base != NULL ) {
        evdns_base_free(ctx->dns_base, 1);
    }

    memset(ctx, 0, sizeof(f4dns_ctx_t));
    free(ctx);
}