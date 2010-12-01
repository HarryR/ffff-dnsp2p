#ifndef OPS_H_
#define OPS_H_

#include "rbtree.h"
#include "ffff.h"

#include <stdint.h>

enum {
    F4OP_MODE_PUT = 10,
    F4OP_MODE_GET = 20
};

struct f4op {
    struct rb_node node;
    char id[20];
    uint8_t mode;
    char *type;
    char *fqn;
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

f4op_t* f4op_find( f4op_ctx_t *ctx, const char *id );

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
