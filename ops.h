#ifndef OPS_H_
#define OPS_H_

#include "rbtree.h"
#include "ffff.h"

#include <event2/dns.h>
#include <stdint.h>

enum {
    F4OP_MODE_PUT = 10,
    F4OP_MODE_GET = 20
};

struct f4op {
    struct rb_node node;

    /**
     * This is the DHT ID for the record to get.
     * The DHT record for "derp.fowehiewhgwe.p2p" is return_value
     * where: f4crypto_hash_fqdn("derp.fowehiewhgwe.p2p", 0, return_value);
     * TODO: what is "replica", why is it always 0?
     */
    unsigned char id[20];

    /**
     * Whether we are getting from or placing in the DHT swarm.
     * F4OP_MODE_PUT or F4OP_MODE_GET
     */
    uint8_t mode;

    /** The type of dns record as a string. EG: "A", "AAAA", "MX" etc. */
    char *type;

    /** The fully qualified domain name EG: "derp.fowehiewhgwe.p2p" */
    char *fqn;

    /** For a GET operation, this is a f4op_get_t object which contains the evdns_server_request. */
    void *data;

    void (*dht_callback)(f4_ctx_t* ctx, struct f4op *op, int event, void *data, size_t data_len);
    void (*dns_callback)(f4_ctx_t* ctx, struct f4op *op, struct evdns_server_request *req);
};
typedef struct f4op f4op_t;

struct f4op_ctx {
    struct rb_root ops;
};
typedef struct f4op_ctx f4op_ctx_t;

f4op_ctx_t *f4op_new_ctx( f4_ctx_t *f4_ctx );
bool f4op_init_ctx( f4op_ctx_t *ctx );
void f4op_free_ctx( f4op_ctx_t *ctx );

f4op_t* f4op_add( f4op_ctx_t *ctx, f4op_t *op );

f4op_t* f4op_find( f4op_ctx_t *ctx, const unsigned char *id );

/**
 * Remove node from op context
 * @param n Node
 * @param op Operation
 * @param ctx Context
 */
void f4op_remove( struct rb_node *n, f4op_t *op, f4op_ctx_t *ctx );

f4op_t *f4op_new( f4op_ctx_t *ctx, uint8_t mode, const char *id );
void  f4op_free( f4op_ctx_t *ctx, f4op_t *op );

#endif
