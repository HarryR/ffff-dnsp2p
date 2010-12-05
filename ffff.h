#ifndef FFFF_H_
#define FFFF_H_

#include <tcutil.h>
#include <tctdb.h>
#include <stdarg.h>
#include <event2/util.h>
#include <event2/dns.h>
#include <event2/http.h>

#include "seccure/curves.h"

/**
 * Errors.
 * f4_ctx->errno is set to one of these by f4_init() of something goes wrong.
 */
enum {
    F4_ERR_CANT_OPEN_DB,
    F4_ERR_CANT_INIT_PEERS,
    F4_ERR_CANT_OPEN_PUBLISH_DB,
    F4_ERR_CANT_OPEN_SOCKET_P2P,
    F4_ERR_CANT_INIT_P2P,
    F4_ERR_CANT_INIT_ADMIN,
    F4_ERR_CANT_INIT_DNS
};

/**
 * A struct representing a peer.
 * Used by f4_start() to inject peers into DHT for bootstrapping.
 */
struct _f4_peer {
    struct sockaddr_storage addr;
    int addr_sz;
};

/**
 * The ffff engine context.
 * This holds all of the state for the ffff engine.
 */
struct f4_ctx {
    bool is_running;
    unsigned int errno;
    struct event_base *base;

    /** Backing database for all persistent state */
    char *db_file;
    TCTDB *db;    

    /** File containing initial list of peers to bootstrap our DHT */
    char *bootstrap_file;
    struct _f4_peer *bootstraps;
    size_t bootstraps_count;

    /** Backend database for all published zones */
    char *publish_file;
    TCTDB *publish_db;

    /** Should be actively publish our own records? */
    bool role_p2p_publish;    
    struct sockaddr_storage listen_p2p;
    int listen_p2p_sz;

    // dht related stuff
    bool dht_done_init;
    evutil_socket_t socket_p2p_dht;
    struct event *socket_p2p_dht_event;

    evutil_socket_t socket_p2p_app;

    /** Should we act as a DNS to P2P resolver? */
    bool role_dns;
    struct sockaddr_storage listen_dns;
    int listen_dns_sz;
    void *dns_ctx;

    /** Should we run an admin interface */
    bool role_admin;
    struct sockaddr_storage listen_admin;
    int listen_admin_sz;
    void *admin_ctx;

    // Operations in progress
    void *op_ctx;

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


/* ------------- functions for injecting configuration ------------- */


/**
 * Set the LibEvent event base.
 * This is the event handler which triggers the appropriate action when requests
 * or announcements come in.
 *
 * @param ctx the ffff engine context.
 * @param base the LibEvent event base.
 */
void f4_set_event_base(f4_ctx_t *ctx, struct event_base *base);

/**
 * Set the name of the file to use for storing node state.
 * This is a backup of a tokyocabinet database which stores the state of the node.
 *
 * @param ctx the ffff engine context.
 * @param db_file a string representation of the path to the file containing the backup
 *                of the database.
 */
void f4_set_db_file(f4_ctx_t *ctx, const char *db_file);

/**
 * Set the name of the file to use for persistant publish storage.
 * This is a backup of a tokyocabinet database which stores all of the announced addresses.
 *
 * @param ctx the ffff engine context.
 * @param publish_db_file a string representation of the path to the file containing the backup
 *                        of the database.
 */
void f4_set_publish_db_file(f4_ctx_t *ctx, const char *publish_db_file);

/**
 * Set address and port for handling incomming DNS requests.
 * The engine will bind to this port and address (UDP) to accept DNS requests.
 *
 * @param ctx the ffff engine context.
 * @param peers_file
 */
void f4_set_peers_file(f4_ctx_t *ctx, const char *peers_file);

/**
 * Set address and port for handling incomming DNS requests.
 * The engine will bind to this port and address (UDP) to accept DNS requests.
 *
 * @param ctx the ffff engine context.
 * @param address a string representation of the address and port to bind to
 *                in <address>:<port> format, for example: 127.0.0.1:53
 */
int f4_set_listen_dns(f4_ctx_t *ctx, const char *address);

/**
 * Set address and port for DHT peer to peer connections.
 * The engine will bind to this port and address (TCP and UDP) to connect to peers.
 * This address and port must be externally reachable.
 *
 * @param ctx the ffff engine context.
 * @param address a string representation of the address and port to bind to
 *                in <address>:<port> format, for example: 0.0.0.0:25578
 */
int f4_set_listen_p2p(f4_ctx_t *ctx, const char *address);

/**
 * Set address and port for administration socket.
 * The engine will bind to this port and address (TCP)
 * to allow the attachment of an administrative controller.
 *
 * @param ctx the ffff engine context.
 * @param address a string representation of the address and port to bind to
 *                in <address>:<port> format, for example: 127.0.0.1:3333
 */
int f4_set_listen_admin(f4_ctx_t *ctx, const char *address);


/* ------------- Functions for running the ffff engine. ------------- */


/**
 * Write a log to stderr.
 *
 * @param ctx the ffff engine context.
 * @param fmt the message to log.
 * @param ... some number of parameters to insert into the message.
 * @see fprintf()
 */
int f4_log(f4_ctx_t *ctx, const char *fmt, ...);

/**
 * Initialize context, opening any sockets, database files etc.
 *
 * @param ctx the ffff engine context.
 * @return 1 on success
 */
bool f4_init( f4_ctx_t *ctx );

/**
 * Manually connect to a peer. Used by f4_init to bootstrap the node.
 *
 * @param ctx the ffff engine context.
 * @param host the host name or ip address is a format recognizable by getaddrinfo()
 * @param port the port number as a base 10 character array or the service name
 *        as recognized by getaddrinfo()
 * @return 0 on success, 1 on failure.
 */
int f4_add_peer( f4_ctx_t *ctx, const char *host, const char *port );

/**
 * Set is_running to true and bootstrap dht.
 *
 * @param ctx the ffff engine context.
 */
void f4_start( f4_ctx_t *ctx );

/**
 * Set is_running to false.
 * This will cause libEvent to stop the loop.
 *
 * @param ctx the ffff engine context.
 */
void f4_stop( f4_ctx_t *ctx );

/**
 * Clean up.
 * Close sockets and free malloc'd memory.
 *
 * @param ctx the ffff engine context.
 */
void f4_free( f4_ctx_t *ctx );

#endif
