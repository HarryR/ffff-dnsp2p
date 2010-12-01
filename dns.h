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

f4dns_ctx_t *f4dns_new(f4_ctx_t *f4_ctx);
bool f4dns_init(f4dns_ctx_t *ctx);
void f4dns_free(f4dns_ctx_t *ctx);

#endif
