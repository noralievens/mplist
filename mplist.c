#include <errno.h>
#include <getopt.h>
#include <mpd/client.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENV_HOST    "MPD_HOST"
#define ENV_PORT    "MPD_PORT"

extern char **environ;
extern int errno ;

/* mpd server properties */
struct {
    char *host;
    unsigned int port;
} server;

/* arg options */
struct option long_options[] = {
    {"host",    required_argument, NULL, 'h'},
    {"port",    required_argument, NULL, 'p'},
    {"quiet",   no_argument,       NULL, 'q'},
    {NULL,      0,                 NULL, 0}
};

struct {
    int quiet;
} options;

struct mpd_connection *mpd_connect()
{
    struct mpd_connection *c = mpd_connection_new(server.host, server.port, 0);
    enum mpd_error mpd_error = mpd_connection_get_error(c);
    if(mpd_error){
        printf("error code %u\n", mpd_error);
        return 0;
    }
    return c;
}

int main(int argc, char **argv)
{
    char* playlist = NULL;
    const char *tracks[100] = { NULL };
    const char **track = tracks;

    /* ENV VAR server host and port */
    while (*environ) {
        if (strstr(*environ, ENV_HOST)) {
            server.host = (*environ)+(strlen(ENV_HOST)+1);
        } else if (strstr(*environ, ENV_PORT)) {
            server.port = atoi((*environ)+(strlen(ENV_PORT)+1));
        }
        environ++;
    }

    /* parse options */
    int oc;
    int oi = 0;
    while ((oc = getopt_long(argc, argv, "h:p:q", long_options, &oi)) != -1) {
        switch (oc) {
            case 'q': options.quiet = 1; break;
            case 'h': server.host = optarg; break;
            case 'p': server.port = atoi(optarg); break;
            default: exit(1);
        }
    }

    /* first arg is playlist, remaining args are tracks */
    while (optind < argc) {
        if (!playlist) {
            playlist = argv[optind];
        } else {
            *track = argv[optind];
            track++;
        }
        optind++;
    }

    /* validate playlist existance */
    if (!playlist) {
        fprintf(stderr, "no playlist given\n");
        return -1;
    }

    /* connect to server and validate */
    struct mpd_connection* c = mpd_connect();
    if (!c) {
        fprintf(stderr, "failed to connect: %s\n", strerror(errno));
        return -1;
    }

    /* fallback to current track if none given */
    if (!tracks[0]) {
        struct mpd_song* curr = mpd_run_current_song(c);
        tracks[0]= mpd_song_get_uri(curr);
    }

    /* add all given tracks to playlist */
    track = tracks;
    while (*track) {
        if (mpd_run_playlist_add(c, playlist, *track)) {
            if (!options.quiet) printf("Added \"%s\" to playlist \"%s\"\n",
                    *track, playlist);
        } else {
            fprintf(stderr, "failed to add \"%s\" to playlist \"%s\"\n",
                    *track, playlist);
            return -1;
        }
        track++;
    }
    return 0;
}
