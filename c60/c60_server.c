#include "c60_internal.h"

/* read map from file, bring up PULL sockets on our IP's */
void *c60 = c60_server_init_fromfile(char *file) {
}

/* establish callback for messages of given prefix (NULL implies any message) */
void c60_server_set_handler(void *c60, char *prefix, size_t len, c60_handler *handler) {
}

/* enter run loop, waiting for messages */
int c60_run(void *handle) {
}
