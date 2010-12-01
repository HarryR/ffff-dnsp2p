#include "ffff.h"
#include "properties.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h> // XXX: windows

#include <event2/util.h>

f4_ctx_t *
f4_new( void ) {
    f4_ctx_t *ctx = calloc(sizeof(f4_ctx_t),1);
    assert( ctx != NULL );
    memset(ctx, 0, sizeof(f4_ctx_t));
    
    return ctx;
}

void
f4_set_event_base(f4_ctx_t *ctx, struct event_base *base) {
    assert( base != NULL );
    ctx->base = base;
}

void
f4_set_db_file(f4_ctx_t *ctx, const char *db_file) {
    assert( db_file != NULL );
    assert( strlen(db_file) );
    ctx->db_file = strdup(db_file);
}

void
f4_set_peers_file(f4_ctx_t *ctx, const char *peers_file) {
    assert( peers_file != NULL );
    assert( strlen(peers_file) );
    ctx->peers_file = strdup(peers_file);
}

void f4_set_publish_db_file(f4_ctx_t *ctx, const char *publish_db_file) {
    assert( publish_db_file != NULL );
    assert( strlen(publish_db_file) );
    ctx->publish_file = publish_db_file;
    ctx->role_p2p_publish = true;
}

int
f4_set_listen_dns(f4_ctx_t *ctx, const char *what) {
    int o = sizeof(struct sockaddr_storage);
    ctx->role_dns = true;
    return evutil_parse_sockaddr_port(what, (struct sockaddr *)&ctx->listen_dns, &o);
}

int f4_set_listen_p2p(f4_ctx_t *ctx, const char *what) {
    int o = sizeof(struct sockaddr_storage);
    return evutil_parse_sockaddr_port(what, (struct sockaddr *)&ctx->listen_p2p, &o);
}

int f4_set_listen_admin(f4_ctx_t *ctx, const char *what) {
    int o = sizeof(struct sockaddr_storage);
    ctx->role_admin = true;
    return evutil_parse_sockaddr_port(what, (struct sockaddr *)&ctx->listen_admin, &o);
}

static void
f4_init_peer_from_addrinfo( f4_ctx_t *ctx, struct addrinfo *res ) {
    ctx->peers_count++;
    ctx->peers = realloc(ctx->peers, sizeof(struct sockaddr_storage) * ctx->peers_count);
    assert( ctx->peers != NULL );
    
    memcpy(&ctx->peers[ctx->peers_count - 1], res->ai_addr, res->ai_addrlen);
}

static int 
f4_init_peer( f4_ctx_t *ctx, struct evkeyval *peer_name ) {
    int error;
    struct addrinfo hints;
    struct addrinfo *res0;
    struct addrinfo *res;    

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    error = getaddrinfo(peer_name->key, peer_name->value, &hints, &res0);
    if( error ) {
        perror(gai_strerror(error));
        return 1;
    }

    for( res = res0; res; res = res->ai_next ) {
        f4_init_peer_from_addrinfo(ctx, res);
    }

    freeaddrinfo(res0);

    return 0;
}

static bool
f4_init_peers( f4_ctx_t *ctx ) {
    struct evkeyvalq peers;
    FILE *fh;
    struct evkeyval *peer;
    size_t invalid_count = 0;
    
    TAILQ_INIT(&peers);
    assert( ctx->peers_file != NULL );
    fh = fopen(ctx->peers_file, "r");
    if( ! fh ) {
        perror("Couldn't open peers file!");
        return false;
    }

    /*
     properties file in the format of:
     peer_address=peer_port
     peer_ddress and peer_port must be in any format parsable by getaddrinfo
     */
    properties_parse_file(fh, &peers);    

    TAILQ_FOREACH(peer, &peers, next) {
        invalid_count += f4_init_peer(ctx, peer);
    }
    
    evhttp_clear_headers(&peers);
    fclose(fh);
    return invalid_count == 0;
}

bool
f4_init(f4_ctx_t *ctx) {
    ctx->db = tctdbnew();
    assert( ctx->db != NULL );
    if( ! tctdbopen(ctx->db, ctx->db_file, TDBOWRITER | TDBOREADER | TDBOCREAT) ) {
        fprintf(stderr, "Couldn't open publish DB: %s\n", tctdberrmsg(tctdbecode(ctx->db)));
        ctx->errno = F4_ERR_CANT_OPEN_DB;
        return false;
    }

    if( ctx->role_p2p_publish ) {
        ctx->publish_db = tctdbnew();
        assert( ctx->publish_db != NULL );
        if( ! tctdbopen(ctx->publish_db, ctx->publish_file, TDBOWRITER | TDBOREADER | TDBOCREAT) ) {
            fprintf(stderr, "Couldn't open publish DB: %s\n", tctdberrmsg(tctdbecode(ctx->publish_db)));
            ctx->errno = F4_ERR_CANT_OPEN_PUBLISH_DB;
            return false;
        }
    }

    if( ! f4_init_peers(ctx) )  {
        ctx->errno = F4_ERR_CANT_INIT_PEERS;
        return false;
    }

    return true;
}

void f4_free( f4_ctx_t *ctx ) {
    assert( ctx != NULL );

    if( ctx->peers ) free(ctx->peers);
    if( ctx->peers_file ) free(ctx->peers_file);
    if( ctx->db_file ) free(ctx->db_file);
    if( ctx->publish_db ) tctdbdel(ctx->publish_db);
    if( ctx->db ) tctdbdel(ctx->db);

    memset(ctx, 0, sizeof(f4_ctx_t));
    free(ctx);
}