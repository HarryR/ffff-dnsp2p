#ifndef FFFF_H_
#define FFFF_H_

#include <tcutil.h>
#include <tctdb.h>
#include <event2/util.h>
#include <event2/dns.h>
#include <event2/http.h>

#include "seccure/curves.h"

enum {
    F4_ERR_CANT_OPEN_DB,
    F4_ERR_CANT_INIT_PEERS,
    F4_ERR_CANT_OPEN_PUBLISH_DB,
    F4_ERR_CANT_OPEN_SOCKET_P2P,
    F4_ERR_CANT_INIT_P2P,
    F4_ERR_CANT_INIT_ADMIN,
    F4_ERR_CANT_INIT_DNS
};

struct f4_ctx {
    bool is_running;
    unsigned int errno;
    struct event_base *base;

    /** Backing database for all persistent state */
    char *db_file;
    TCTDB *db;    

    /** File containing initial list of peers to bootstrap our DHT */
    char *peers_file;
    struct sockaddr_storage *peers;
    size_t peers_count;

    /** Backend database for all published zones */
    char *publish_file;
    TCTDB *publish_db;

    /** Should be actively publish our own records? */
    bool role_p2p_publish;    
    struct sockaddr_storage listen_p2p;

    // dht related stuff
    bool dht_done_init;
    evutil_socket_t socket_p2p_dht;
    struct event *socket_p2p_dht_event;

    evutil_socket_t socket_p2p_app;

    /** Should we act as a DNS to P2P resolver? */
    bool role_dns;
    struct sockaddr_storage listen_dns;
    void *dns_ctx;

    /** Should we run an admin interface */
    bool role_admin;
    struct sockaddr_storage listen_admin;
    void *admin_ctx;

    // Crypto shizzle, yay
    struct curve_params *cp;
    char private_key[32];
    char public_key[20];
};
typedef struct f4_ctx f4_ctx_t;

/**
 * Create a new F4 context
 *
 * @param base Event base
 * @param db_file Database file used for storage
 * @param peers_file Initial set of peers to connext to
 * @return newly created context
 */
f4_ctx_t* f4_new( void );

void f4_set_event_base(f4_ctx_t *ctx, struct event_base *base);
void f4_set_db_file(f4_ctx_t *ctx, const char *db_file);
void f4_set_publish_db_file(f4_ctx_t *ctx, const char *publish_db_file);
void f4_set_peers_file(f4_ctx_t *ctx, const char *peers_file);
int f4_set_listen_dns(f4_ctx_t *ctx, const char *what);
int f4_set_listen_p2p(f4_ctx_t *ctx, const char *what);
int f4_set_listen_admin(f4_ctx_t *ctx, const char *what);

/**
 * Initialize context, opening any sockets, database files etc.
 * @param ctx
 * @return 1 on success
 */
bool f4_init(f4_ctx_t *ctx);

void f4_free( f4_ctx_t *ctx );

#endif
