#include <assert.h>
#include <stdlib.h>

#include "ops.h"

f4op_ctx_t *
f4op_new_ctx( f4_ctx_t *f4_ctx ) {
    assert( f4_ctx != NULL );
    f4op_ctx_t *ctx = (f4op_ctx_t *)calloc(sizeof(f4op_ctx_t), 1);
    memset(ctx, 0, sizeof(f4op_ctx_t));
    ctx->ops.rb_node = NULL;
    return ctx;
}

bool
f4op_init_ctx( f4op_ctx_t *ctx ) {
    assert( ctx != NULL );
    return true;
}

f4op_t*
f4op_add( f4op_ctx_t *ctx, f4op_t *op ) {
    struct rb_node **p = &ctx->ops.rb_node;
    struct rb_node *parent = NULL;
    f4op_t *found_op;

    while( *p ) {
        int comp;
        parent = *p;
        found_op = rb_entry(parent, f4op_t, node);
        comp = memcmp(op->id, found_op->id, 20);
        if( comp < 0 )  {
            p = &(*p)->rb_left;
        }
        else if( comp > 0 ) {
            p = &(*p)->rb_right;
        }
        else {
            return found_op;
        }
    }

    rb_link_node(&op->node, parent, p);
    return NULL;
}

void
f4op_remove( struct rb_node *n, f4op_t *op, f4op_ctx_t *ctx ) {
    rb_erase(n, &ctx->ops);
    f4op_free(ctx, op);
}

f4op_t*
f4op_find( f4op_ctx_t *ctx, const char *id ) {
    struct rb_node *n = ctx->ops.rb_node;
    f4op_t *op;

    while( n ) {
        int comp;
        op = rb_entry(n, f4op_t, node);
        comp = memcmp(id, op->id, 20);
        if( comp <  0 ) {
            n = n->rb_left;
        }
        else if( comp > 0 ) {
            n = n->rb_right;
        }
        else {
            return op;
        }
    }
    return NULL;
}

void
f4op_free_ctx( f4op_ctx_t *ctx ) {
    struct rb_node *p, *tmp;
    f4op_t *op = NULL;

    p = rb_first(&ctx->ops);
    while( p ) {
        op = rb_entry(p, f4op_t, node);
        tmp = p;
        p = rb_next(tmp);

        f4op_remove(tmp, op, ctx);
    }

    memset(ctx, 0, sizeof(f4op_ctx_t));
    free(ctx);
}

f4op_t *
f4op_new( f4op_ctx_t *ctx, uint8_t mode, const char *id ) {
    assert( ctx != NULL );
    f4op_t *op = (f4op_t*)calloc(sizeof(f4op_t),1);
    op->mode = mode;
    memcpy(op->id, id, 20);
    return op;
}

void
f4op_free( f4op_ctx_t *ctx, f4op_t *op ) {
    assert( ctx != NULL );

    // TODO: cancel any pending operations
    
    if( op->fqn != NULL ) free(op->fqn);
    if( op->type != NULL ) free(op->type);
    free(op);
}