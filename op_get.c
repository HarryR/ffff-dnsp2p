#include <assert.h>
#include <stdlib.h>

#include "dht/dht.h"
#include "op_get.h"
#include "ops.h"

static void
opget_dht_callback(f4_ctx_t* ctx, struct f4op *op, int event, void *data, size_t data_len) {
    assert( false );
    // TODO: implement me
}

static void
opget_dns_callback(f4_ctx_t* ctx, struct f4op *op, struct evdns_server_request *req) {
    assert( ctx != NULL );
    assert( op != NULL );
    assert( req != NULL );
    f4op_get_t *get_op = (f4op_get_t *)op->data;
    get_op->request = req;

    // XXX: re-workt dht_search to use ipv4/ipv6 as available rather than
    // explicitly specifying AF_INET or AF_INET6
    // XXX: Nor do we care about a callback, that's handled elsewhere
    dht_search( op->id, 0, AF_INET, NULL, NULL );
}

f4op_t *
f4op_get_new(f4op_t *op) {
    f4op_get_t *get_ctx;
    assert( op->mode == F4OP_MODE_GET );
    get_ctx = op->data = calloc(sizeof(f4op_get_t),1);
    assert( get_ctx != NULL );    
    op->dht_callback = opget_dht_callback;
    op->dns_callback = opget_dns_callback;
    return op;
}

void
f4op_get_free(f4op_t *op) {
    f4op_get_t *get_ctx;
    assert( op->mode == F4OP_MODE_GET );
    get_ctx = (f4op_get_t*)op->data;
    assert( get_ctx->request == NULL );
    memset(get_ctx, 0, sizeof(f4op_get_t));
    free( get_ctx );
    op->data = NULL;
}