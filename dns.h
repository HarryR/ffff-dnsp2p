#ifndef DNS_H_
#define DNS_H_

#include "ffff.h"

#include <event2/util.h>
#include <event2/dns.h>

/**
 * The ffff DNS engine context.
 * This holds a reference to the main ffff engine context.
 * It also holds references to parts which will need to be
 * shutdown when the server is stopped.
 */
struct f4dns_ctx {

    /** The libevent DNS context. This needs to be freed when the engine is stopped. */
    struct evdns_base *dns_base;

    /** The main ffff engine context. */
    f4_ctx_t *f4;

    /** The socket which the DNS server is bound to. Not currently used. */
    evutil_socket_t sock;

    /** The server context. This is needed in order to be able to shut down the server. */
    struct evdns_server_port *server;
};
typedef struct f4dns_ctx f4dns_ctx_t;

/**
 * Get the hash for a request
 * sets the data at location "hash_return" to the value of:
 * sha1("F4DNS::" + fqn)
 *
 * @param request_type e.g. "A" or "MX"
 * @param fqn "derp.fowehiewhgwe.p2p"
 * @param hash_return pointer to a 20 byte char array
 * @return hash_return
 */
char *f4dns_hash( const char *fqn, char *hash_return );

/**
 * Create a new ffff DNS context.
 * Nothing is initialized only allocation of memory.
 *
 * @param f4_ctx the underlying ffff engine context.
 */
f4dns_ctx_t *f4dns_new(f4_ctx_t *f4_ctx);

/**
 * Initialize a newly created ffff DNS context.
 * The DNS listening socket is bound to.
 *
 * @param f4_ctx the underlying ffff engine context.
 */
bool f4dns_init(f4dns_ctx_t *ctx);

/**
 * Shutdown the DNS server and free the allocated memory.
 *
 * @param f4_ctx the underlying ffff engine context.
 */
void f4dns_free(f4dns_ctx_t *ctx);

bool f4dns_is_valid(const char *fqdn);

#endif
