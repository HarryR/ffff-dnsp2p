#include <assert.h>
#include <stdlib.h>

#include "op_get.h"
#include "ops.h"

static void
opget_dht_callback(f4_ctx_t* ctx, struct f4op *op, int event, void *data, size_t data_len) {
    assert( false );
    // TODO: implement me
}

f4op_t *
f4op_get_new(f4op_t *op, struct evdns_server_request *req) {
    f4op_get_t *get_ctx;
    assert( op->mode == F4OP_MODE_GET );
    get_ctx = op->data = calloc(sizeof(f4op_get_t),1);
    assert( get_ctx != NULL );
    get_ctx->request = req;
    op->dht_callback = opget_dht_callback;
    return op;
}

void
f4op_get_free(f4op_t *op) {
    f4op_get_t *get_ctx;
    assert( op->mode == F4OP_MODE_GET );
    get_ctx = (f4op_get_t*)op->data;
    assert( get_ctx->request == NULL );
    free( get_ctx );
    op->data = NULL;
}