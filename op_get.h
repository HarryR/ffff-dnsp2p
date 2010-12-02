#ifndef OP_GET_H_
#define OP_GET_H_

#include "ops.h"

#include <event2/dns.h>

struct f4op_get {
    struct evdns_server_request *request;
};
typedef struct f4op_get f4op_get_t;

f4op_t *f4op_get_new(f4op_t *op, struct evdns_server_request *req);

void f4op_get_free(f4op_t *op);

#endif
