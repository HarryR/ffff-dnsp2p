#include "ffff.h"
#include "admin.h"
#include "dns.h"
#include "seccure/protocol.h"
#include "properties.h"
#include "dht/dht.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h> // XXX: windows

#include <event2/event.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include "ops.h"

f4_ctx_t *
f4_new( void ) {
    f4_ctx_t *ctx = calloc(sizeof(f4_ctx_t),1);
    assert( ctx != NULL );
    memset(ctx, 0, sizeof(f4_ctx_t));  
    ctx->socket_p2p_app = -1;
    ctx->socket_p2p_dht = -1;
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
    if( ctx->db_file ) free(ctx->db_file);
    ctx->db_file = strdup(db_file);
}

void
f4_set_peers_file(f4_ctx_t *ctx, const char *peers_file) {
    assert( peers_file != NULL );
    assert( strlen(peers_file) );
    if( ctx->peers_file ) free(ctx->peers_file);
    ctx->peers_file = strdup(peers_file);
}

void f4_set_publish_db_file(f4_ctx_t *ctx, const char *publish_db_file) {
    assert( publish_db_file != NULL );
    assert( strlen(publish_db_file) );
    if( ctx->publish_file ) free(ctx->publish_file);
    ctx->publish_file = strdup(publish_db_file);
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

static bool
socket_v6_only( evutil_socket_t s6 ) {
    int rc;
    int val = 1;

    rc = setsockopt(s6, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&val, sizeof(val));
    return rc == 0;
}

// Externals from dht.c
int
dht_random_bytes(void *buf, size_t size) {
    gcry_randomize(buf, size, GCRY_STRONG_RANDOM);
    return 0;
}
void
dht_hash(void *hash_return, int hash_size,
         const void *v1, int len1,
         const void *v2, int len2,
         const void *v3, int len3)
{
    gcry_error_t err;
    gcry_md_hd_t mh;
    char *md;

    err = gcry_md_open(&mh, GCRY_MD_MD5, GCRY_MD_FLAG_SECURE);
    assert( ! gcry_err_code(err) );

    gcry_md_write(mh, v1, len1);
    gcry_md_write(mh, v2, len2);
    gcry_md_write(mh, v3, len3);

    gcry_md_final(mh);

    md = (char*)gcry_md_read(mh, 0);
    memcpy(hash_return, md, hash_size > 16 ? 16 : hash_size);
    gcry_md_close(mh);
}

static void
f4_cb_dht(void *_ctx, int event,
             unsigned char *info_hash,
             void *data, size_t data_len)
{
    f4_ctx_t *ctx = (f4_ctx_t*)ctx;

    // TODO: match info_hash to a pending DNS query/share operation
    // We can then create a connection to the nodes given in 'data' to complete
    // the operation.
    assert( false );
}

static void
f4_cb_dht_read( evutil_socket_t s, short event, void *_ctx ) {
    f4_ctx_t *ctx = (f4_ctx_t*)_ctx;
    time_t tosleep;
    dht_periodic(event == EV_WRITE, &tosleep, f4_cb_dht, ctx);
}

static bool
f4_init_p2p( f4_ctx_t *ctx ) {
    // XXX: for now only bind on DHT port until the app side is worked out
    /*
    ctx->socket_p2p_app = socket(ctx->listen_p2p.ss_family, SOCK_STREAM, 0);
    if( ctx->socket_p2p_app == -1 ) {
        perror("Canot p2p app socket()");
        ctx->errno = F4_ERR_CANT_OPEN_SOCKET_P2P;
        return false;
    }
    evutil_make_socket_nonblocking(ctx->socket_p2p_app);
    evutil_make_listen_socket_reuseable(ctx->socket_p2p_app);

    if( bind(ctx->socket_p2p_app, (struct sockaddr *)&ctx->listen_p2p, sizeof(struct sockaddr_storage)) != 0 ) {
        perror("Cannot bind() p2p app");
        ctx->errno = F4_ERR_CANT_OPEN_SOCKET_P2P;
        return false;
    }
    */

    ctx->socket_p2p_dht = socket(ctx->listen_p2p.ss_family, SOCK_DGRAM, 0);
    if( ctx->socket_p2p_dht == -1 ) {
        perror("Cannot p2p dht socket()");
        ctx->errno = F4_ERR_CANT_OPEN_SOCKET_P2P;
        return false;
    }
    // XXX: we need separate sockets for v4 and v6 DHT sockets
    // TODO: fully integrate the DHT library with libevent
    if( ctx->listen_p2p.ss_family == AF_INET6 ) {
        socket_v6_only( ctx->socket_p2p_dht );
    }
    evutil_make_socket_nonblocking(ctx->socket_p2p_dht);
    evutil_make_listen_socket_reuseable(ctx->socket_p2p_dht);

    if( bind(ctx->socket_p2p_dht, (struct sockaddr *)&ctx->listen_p2p, sizeof(struct sockaddr_storage)) != 0 ) {
        perror("Cannot bind() p2p dht");
        ctx->errno = F4_ERR_CANT_OPEN_SOCKET_P2P;
        return false;
    }

    evutil_socket_t s4 = -1, s6 = -1;

    if( ctx->listen_p2p.ss_family == AF_INET6 ) {
        s6 = ctx->socket_p2p_dht;
    }
    else {
        assert( ctx->listen_p2p.ss_family == AF_INET );
        s4 = ctx->socket_p2p_dht;
    }

    if( dht_init(s4, s6, ctx->public_key, (unsigned char*)"P2NS") < 0 ) {
        perror("dht_init()");
        return false;
    }
    ctx->dht_done_init = true;

    // XXX: dht isn't fully integrated with libevent
    ctx->socket_p2p_dht_event = event_new(ctx->base, ctx->socket_p2p_dht, EV_READ | EV_PERSIST, f4_cb_dht_read, ctx);
    event_add(ctx->socket_p2p_dht_event, NULL);

    return true;
}

static void
f4_init_crypto(f4_ctx_t *ctx) {
    struct affine_point private_P;
    gcry_mpi_t private_d;

    ctx->cp = curve_by_pk_len_compact(20);
    assert( ctx->cp != NULL );

    gcry_randomize(&ctx->private_key[0], sizeof(ctx->private_key), GCRY_STRONG_RANDOM);
    private_d = hash_to_exponent(ctx->private_key, ctx->cp);
    private_P = pointmul(&ctx->cp->dp.base, private_d, &ctx->cp->dp);
    gcry_mpi_release(private_d);

    compress_to_string(ctx->public_key, DF_BIN, &private_P, ctx->cp);
    point_release(&private_P);
}

bool
f4_init(f4_ctx_t *ctx) {
    f4_init_crypto(ctx);

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

    ctx->op_ctx = f4op_new_ctx(ctx);
    f4op_init_ctx(ctx->op_ctx);

    if( ! f4_init_peers(ctx) )  {
        ctx->errno = F4_ERR_CANT_INIT_PEERS;
        return false;
    }

    if( ! f4_init_p2p(ctx) ) {
        ctx->errno = F4_ERR_CANT_INIT_P2P;
        return false;
    }

    if( ctx->role_admin ) {
        ctx->admin_ctx = f4admin_new(ctx);
        if( ! f4admin_init(ctx->admin_ctx) ) {
            ctx->errno = F4_ERR_CANT_INIT_ADMIN;
            return false;
        }
    }

    if( ctx->role_dns ) {
        ctx->dns_ctx = f4dns_new(ctx);
        if( ! f4dns_init(ctx->dns_ctx) ) {
            ctx->errno = F4_ERR_CANT_INIT_DNS;
            return false;
        }
    }

    return true;
}

void f4_free( f4_ctx_t *ctx ) {
    assert( ctx != NULL );

    if( ctx->admin_ctx ) {
        f4admin_free(ctx->admin_ctx);
    }

    if( ctx->dht_done_init ) {
        event_del(ctx->socket_p2p_dht_event);
        dht_uninit(1);
    }

    if( ctx->cp != NULL ) curve_release(ctx->cp);

    if( ctx->socket_p2p_app != -1 ) EVUTIL_CLOSESOCKET(ctx->socket_p2p_app);
    if( ctx->socket_p2p_dht != -1 ) EVUTIL_CLOSESOCKET(ctx->socket_p2p_dht);

    if( ctx->peers ) free(ctx->peers);
    if( ctx->peers_file ) free(ctx->peers_file);
    if( ctx->db_file ) free(ctx->db_file);
    if( ctx->publish_db ) tctdbdel(ctx->publish_db);
    if( ctx->db ) tctdbdel(ctx->db);

    memset(ctx, 0, sizeof(f4_ctx_t));
    free(ctx);
}
