#include "dns.h"
#include "ops.h"

#include <assert.h>
#include <gcrypt.h>
#include <event2/dns.h>
#include <event2/dns_struct.h>

char *
f4dns_hash( const char *request_type, const char *fqn, char *hash_return ) {
    gcry_error_t err;
    gcry_md_hd_t mh;
    char *md;

    err = gcry_md_open(&mh, GCRY_MD_SHA1, GCRY_MD_FLAG_SECURE);
    assert( ! gcry_err_code(err) );

    gcry_md_write(mh, "F4DNS", strlen("F4DNS"));
    gcry_md_write(mh, request_type, strlen(request_type));
    gcry_md_write(mh, fqn, strlen(fqn));

    gcry_md_final(mh);

    md = (char*)gcry_md_read(mh, 0);
    memcpy(hash_return, md, 20);
    gcry_md_close(mh);
    return hash_return;
}

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
    f4dns_ctx_t *ctx = (f4dns_ctx_t *)_ctx;
    int i;

    for( i = 0; i < req->nquestions; i++ ) {
        char op_id[20];
        char *r_type;
        char *r_fqdn = req->questions[i]->name;

        switch( req->questions[i]->type ) {
        case EVDNS_TYPE_A:
            r_type = "A";
            break;

        case EVDNS_TYPE_AAAA:
            r_type = "AAAA";
            break;

        case EVDNS_TYPE_MX:
            r_type = "MX";
            break;

        case EVDNS_TYPE_NS:
            r_type = "NS";
            break;

        case EVDNS_TYPE_PTR:
            r_type = "PTR";
            break;

        case EVDNS_TYPE_SOA:
            r_type = "SOA";
            break;

        case EVDNS_TYPE_TXT:
            r_type = "TXT";
            break;

        default:
            // XXX: No dude, we shouldn't do this!
            r_type = "ANY";
            break;
        }

        f4dns_hash(r_type, r_fqdn, op_id);
        f4op_t *op = f4op_new(ctx->f4->op_ctx, F4OP_MODE_GET, op_id);
        op->type = strdup(r_type);
        op->fqn = strdup(r_fqdn);
        f4op_add(ctx->f4->op_ctx, op);

        // TODO: add callbacks into f4op_t so it can start the resolv process
        // This will start a DHT search
    }

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
