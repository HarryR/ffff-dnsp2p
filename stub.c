#include "ffff.h"
#include "dht/dht.h"

#include <stdio.h>
#include <stdlib.h>
#include <event2/event.h>
#include <unistd.h>
#include <libgen.h>

/**
 * This is the main loop.
 */
static int
run_stuff( f4_ctx_t* ctx ) {
    struct timeval timeout = {5, 5000};
    f4_start(ctx);
    while( ctx->is_running ) {
	event_base_loopexit(ctx->base, &timeout);
	event_base_loop(ctx->base, 0);
        dht_dump_tables(stdout);
    }
    f4_stop(ctx);
    return -1;
}

/**
 * Print the help message.
 * @param argv0 the name of the program.
 */
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
    fprintf(stderr, " -b <file>         Peer file containing bootstrap nodes\n");
    fprintf(stderr, " -h                Show this help\n");
}

/**
 * Main function.
 * @param argc the number of arguments at the command line.
 * @param argv the arguments themselves.
 */
int
main(int argc, char** argv) {
    // The error code from gcrypt
    gcry_error_t err;

    // The ffff object.
    f4_ctx_t *ctx;

    // The LibEvent object. This is passed to the ffff object.
    struct event_base *base;
    base = event_base_new();

    ctx = f4_new();

    int ret = EXIT_FAILURE;

    // If no arguments then print the help message.
    bool should_show_help = (argc == 1);

    // Init gcrypt.
    err = gcry_control(GCRYCTL_INIT_SECMEM, 1);
    if (gcry_err_code(err)) {
      fprintf(stderr, "Cannot enable gcrypt's secure memory management\n");
      exit(EXIT_FAILURE);
    }

    err = gcry_control(GCRYCTL_USE_SECURE_RNDPOOL, 1);
    if (gcry_err_code(err)) {
      fprintf(stderr, "Cannot enable gcrypt's secure random number generator\n");
      exit(EXIT_FAILURE);
    }

    // TODO: find unused local port for P2P listen, and set using f4_set_listen_p2p
    // Setup some reasonable defaults
    f4_set_listen_p2p(ctx, "[::]:14010");
    f4_set_listen_admin(ctx, "[::]:14040");

    // char* optarg; This is set by getopt().

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

    if( ! should_show_help && ! ctx->db_file ) {
        fprintf(stderr, "Error: must specify status database file, with -s\n");
        should_show_help = true;
    }

    if( should_show_help ) {
        // Show help and exit.
        show_help(argv[0]);
    }
    else {
        // Otherwise start the engine.

        if( ! ctx->bootstrap_file ) {
            fprintf(stderr, "Warning: using default bootstrap peers, override with -b\n");
            f4_add_peer(ctx, "router.bittorrent.com", "6881");
            if( ctx->listen_p2p.ss_family == AF_INET6 ) {
                f4_add_peer(ctx, "dht.wifi.pps.jussieu.fr", "6881");
            }
        }

        // Put the libEvent into the ffff object.
        f4_set_event_base(ctx, base);

        // Initialize context, opening any sockets, database files etc.
        if( f4_init(ctx) ) {
            // Then enter the main loop.
            ret = run_stuff(ctx);
        }
        else {
            // Or fail.
            fprintf(stderr, "Couldn't fully init F4 subsystem\n");
            ret = EXIT_FAILURE;
        }
    }

    // Clean up.
    f4_free(ctx);
    event_base_free(base);
    return (ret);
}

