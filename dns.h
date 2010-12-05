#ifndef DNS_H_
#define DNS_H_

#include "ffff.h"

#include <event2/util.h>
#include <event2/dns.h>

struct f4dns_ctx {
    struct evdns_base *dns_base;
    f4_ctx_t *f4;
    evutil_socket_t sock;
    struct evdns_server_port *server;
};
typedef struct f4dns_ctx f4dns_ctx_t;

/**
 * Get the hash for a request
 *
 * @param request_type e.g. "A" or "MX"
 * @param fqn "derp.fowehiewhgwe.p2p"
 * @param hash_return pointer to a 20 byte char array
 * @return hash_return
 */
char *f4dns_hash( const char *fqn, char *hash_return );
f4dns_ctx_t *f4dns_new(f4_ctx_t *f4_ctx);
bool f4dns_init(f4dns_ctx_t *ctx);
void f4dns_free(f4dns_ctx_t *ctx);

bool f4dns_is_valid(const char *fqdn);

#endif
