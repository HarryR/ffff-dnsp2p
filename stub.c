#include "ffff.h"

#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include <unistd.h>
#include <libgen.h>

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
    fprintf(stderr, " -h                Show this help\n");
}

int
main(int argc, char** argv) {
    f4_ctx_t *ctx;
    struct event_base *base;
    base = event_base_new();
    ctx = f4_new();
    int ret = EXIT_SUCCESS;
    bool should_show_help = (argc == 1);

    // TODO: find unused local port for P2P listen, and set using f4_set_listen_p2p

    int ch;
    while( (ch = getopt(argc, argv, "D:A:P:s:p:h")) != -1 ) {
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

        case 'h':
            should_show_help = true;
            break;
        }
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

