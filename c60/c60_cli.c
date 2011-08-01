#include <stdlib.h>
#include <zmq.h>
#include "c60_internal.h"
#include "utstring.h"

/* read mapping from file. open push conn to each c60 node. */
void *c60_client_init_fromfile(char *file, UT_string *err) {
  c60_t *c60=NULL;
  int rc = -1;

  if ( (c60 = calloc(1,sizeof(*c60))) == NULL) goto done;
  if ( (c60->zcontext = zmq_init(1)) == NULL) goto done;
  if (c60_load_map(c60,file,err) == -1) goto done;

  rc = 0;

 done:
  if (rc == -1) {
    if (c60) free(c60);
    /* TODO deep clean up c60 if non-null */
    c60 = NULL;
  }
  return c60;
}

/* hash dest via the map; push msg to appropriate node */
int c60_send(char *dest, char *msg, size_t len) {
  return -1;
}


