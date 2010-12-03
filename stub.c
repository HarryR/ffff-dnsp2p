#include "ffff.h"

#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include <unistd.h>
#include <libgen.h>
#include "admin.h"

static int
run_stuff( f4_ctx_t* ctx ) {
    ctx->is_running = true;
    while( ctx->is_running ) {
        event_base_loop(ctx->base, EVLOOP_ONCE);
    }
    return -1;
}

static void
show_help( char *argv0 ) {
    const char *derp = basename(argv0);

    fprintf(stderr, "Usage: %s <options ...>\n\n", derp);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, " -D <addr:port>    Listen address & port for DNS resolver (UDP)\n");
    fprintf(stderr, " -A <addr:port>    Listen address & port for admin HTTP port (TCP)\n");
    fprintf(stderr, " -P <addr:port>    Listen address & port for P2P connectivity (TCP+UDP)\n");
    fprintf(stderr, " -s <file>         DB file for node state storage\n");
    fprintf(stderr, " -p <file>         DB file for persistent publish storage\n");
    fprintf(stderr, " -b <file>         Peer list bootstrap file\n");
    fprintf(stderr, " -h                Show this help\n");
}

int
main(int argc, char** argv) {
    gcry_error_t err;
    f4_ctx_t *ctx;
    struct event_base *base;
    base = event_base_new();
    ctx = f4_new();
    int ret = EXIT_FAILURE;
    bool should_show_help = (argc == 1);

    err = gcry_control(GCRYCTL_INIT_SECMEM, 1);
    if (gcry_err_code(err)) {
      LOG("Cannot enable gcrypt's secure memory management\n");
      exit(EXIT_FAILURE);
    }

    err = gcry_control(GCRYCTL_USE_SECURE_RNDPOOL, 1);
    if (gcry_err_code(err)) {
      LOG("Cannot enable gcrypt's secure random number generator\n");
      exit(EXIT_FAILURE);
    }

    // TODO: find unused local port for P2P listen, and set using f4_set_listen_p2p
    // Setup some reasonable defaults
    f4_set_listen_p2p(ctx, "[::]:14010");
    f4_set_listen_admin(ctx, "[::]:14040");

    int ch;
    while( (ch = getopt(argc, argv, "D:A:P:s:p:b:h")) != -1 ) {
        switch(ch) {
        case 'D':
            f4_set_listen_dns(ctx, optarg);
            break;
        case 'A':
            f4_set_listen_admin(ctx, optarg);
            break;

        case 'P':
            f4_set_listen_p2p(ctx, optarg);
            break;

        case 's':
            f4_set_db_file(ctx, optarg);
            break;

        case 'p':
            f4_set_publish_db_file(ctx, optarg);
            break;

        case 'b':
            f4_set_peers_file(ctx, optarg);
            break;

        case 'h':
            should_show_help = true;
            break;
        }
    }

    if( ! ctx->db_file ) {
        fprintf(stderr, "Error: must specify status database file, with -s\n");
        should_show_help = true;
    }

    if( ! ctx->peers_file ) {
        fprintf(stderr, "Error: must specify bootstrap peers file, with -b\n");
        should_show_help = true;
    }

    if( ! should_show_help ) {
        f4_set_event_base(ctx, base);
        if( ! f4_init(ctx) ) {
            fprintf(stderr, "Couldn't fully init F4 subsystem\n");
            ret = EXIT_FAILURE;
        }
        else {
            ret = run_stuff(ctx);
        }
    }
    else {
        show_help(argv[0]);
    }

    f4_free(ctx);
    event_base_free(base);
    return (ret);
}

