#include <errno.h>
#include <mpd/client.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

extern char **environ;
extern int errno ;
extern char *optarg;
extern int optind, opterr, optopt;

char *strstr_after(const char* haystack, const char* needle)
{
    char *start = strstr(haystack, needle);
    if (!start) return NULL;
    else return start+strlen(needle);
}

struct server {
    char *host;
    unsigned int port;
} server;

struct options {
    int quiet;
} options;

struct mpd_connection* conn()
{
    struct mpd_connection* c = mpd_connection_new(server.host, server.port, 0);
    enum mpd_error err = mpd_connection_get_error(c);
    if(err){
        printf("error code %u\n", err);
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
        char *envline = NULL;
        if ((envline = strstr_after(*environ, "MPD_HOST="))) {
            server.host = envline;
        } else if ((envline = strstr_after(*environ, "MPD_PORT="))) {
            server.port = atoi(envline);
        }
        environ++;
    }

    /* parse options */

    static struct option long_options[] = {
        /*   NAME       ARGUMENT           FLAG  SHORTNAME */
        {"host",    required_argument, NULL, 'h'},
        {"port",    required_argument, NULL, 'p'},
        {"quiet",   no_argument,       NULL, 'q'},
        {NULL,      0,                 NULL, 0}
    };

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

    struct mpd_connection* c = conn();

    /* validate mpd connection */
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
