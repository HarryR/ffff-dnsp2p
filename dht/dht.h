/*
Copyright (c) 2009 by Juliusz Chroboczek

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef DHT_DHT_H_
#define DHT_DHT_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#include "libbenc/benc.h"
#include "libbenc/bencode.h"

typedef void
dht_callback(void *closure, int event,
             unsigned char *info_hash,
             void *data, size_t data_len);

#define DHT_EVENT_NONE 0
#define DHT_EVENT_VALUES 1
#define DHT_EVENT_VALUES6 2
#define DHT_EVENT_SEARCH_DONE 3
#define DHT_EVENT_SEARCH_DONE6 4

extern FILE *dht_debug;

struct sockaddr_in6;
struct sockaddr_in;

int dht_init(int s, int s6, const char *id, const unsigned char *v);
int dht_insert_node(const unsigned char *id, struct sockaddr *sa, int salen);
int dht_ping_node(struct sockaddr *sa, int salen);
int dht_periodic(int available, time_t *tosleep,
                 dht_callback *callback, void *closure);
int dht_search(const unsigned char *id, int port, int af,
               dht_callback *callback, void *closure);
int dht_nodes(int af,
              int *good_return, int *dubious_return, int *cached_return,
              int *incoming_return);
void dht_dump_tables(FILE *f);
int dht_get_nodes(struct sockaddr_in *sin, int *num,
                  struct sockaddr_in6 *sin6, int *num6);
int dht_uninit(int dofree);

/* This must be provided by the user. */
void dht_hash(void *hash_return, int hash_size,
              const void *v1, int len1,
              const void *v2, int len2,
              const void *v3, int len3);
int dht_random_bytes(void *buf, size_t size);

enum dht_message_type {
	DHT_MSG_ERR = 0,
	DHT_MSG_QUERY = 1,
	DHT_MSG_RESPONSE = 2,
};
typedef enum dht_message_type dht_message_type_e;

enum dht_query_type {
	DHT_QUERY_ERROR = 0,
	DHT_QUERY_REPLY = 1,
	DHT_QUERY_PING = 2,
	DHT_QUERY_FIND_NODE = 3,
	DHT_QUERY_GET_PEERS = 4,
	DHT_QUERY_ANNOUNCE_PEER = 5,
};
typedef enum dht_query_type dht_query_type_e;

struct _dht_message {
    bbuf_t *buf;
    bobj_t *obj;

	dht_message_type_e type;
	dht_query_type_e query;
	
    char *t;
	int t_len;
    char *y;
	char *q;
	bobj_t *a;
    bobj_t *r;
	bobj_t *e;
	
	char *arg_id;
};
typedef struct _dht_message dht_message_t;

struct _dht_periodic_state {
    dht_message_t *msg;
    struct sockaddr_storage source_storage;
    struct sockaddr *source;
    socklen_t sourcelen;
    unsigned short ttid;
    dht_callback *callback;
    void *closure;
    char buf[1536];
    int rc;
};
typedef struct _dht_periodic_state dht_periodic_state_t;

dht_message_t *dht_message_parse(const char *buf, int buflen);
void dht_message_free(dht_message_t *m);

#endif
