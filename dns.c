#include "dns.h"

#include <assert.h>

f4dns_ctx_t *
f4dns_new(f4_ctx_t *f4_ctx) {
    f4dns_ctx_t *ctx = (f4dns_ctx_t *)calloc(sizeof(f4dns_ctx_t), 1);
    memset(ctx, 0, sizeof(f4dns_ctx_t));
    ctx->f4 = f4_ctx;
    ctx->sock = -1;
    return ctx;
}

static void 
_f4dns_cb_dnsserver(struct evdns_server_request *req, void *_ctx) {
    assert( _ctx != NULL );
    f4dns_ctx_t *ctx = (f4dns_ctx_t *)ctx;

    // TODO: setup new P2P dns lookup/resolve operation
    // XXX: for now just reply
    evdns_server_request_respond(req, 0);
}

bool
f4dns_init(f4dns_ctx_t *ctx) {
    ctx->dns_base = evdns_base_new( ctx->f4->base, 1 );
    assert( ctx->dns_base != NULL );

    ctx->sock = socket(ctx->f4->listen_dns.ss_family, SOCK_DGRAM, 0);
    evutil_make_socket_nonblocking(ctx->sock);

    if( bind(ctx->sock, (struct sockaddr*)&ctx->f4->listen_dns, sizeof(struct sockaddr_storage)) != 0 ) {
        perror("bind()");
        return false;
    }

    ctx->server = evdns_add_server_port_with_base(ctx->f4->base, ctx->sock, 0, _f4dns_cb_dnsserver, ctx);

    return true;
}

void
f4dns_free(f4dns_ctx_t *ctx) {
    assert( ctx != NULL );

    if( ctx->server != NULL ) {
        evdns_close_server_port(ctx->server);
    }

    if( ctx->dns_base != NULL ) {
        evdns_base_free(ctx->dns_base, 1);
    }

    memset(ctx, 0, sizeof(f4dns_ctx_t));
    free(ctx);
}