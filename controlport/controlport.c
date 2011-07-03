#include "controlport.h"

typedef struct {

  cp_cmd_t *cmds; // hash table of commands
  int connects;   // number of accepts 
  int fd;         // listener

  /* per client stuff. we're an iterative server, so one client at a time.
     so we can just keep the client state right here with server state */

  int cd;        // client fd

} cp_t;

void *cp_init(char *path, cp_cmd_t *cmds, int timeout) {
}

int cp_add_cmd(void *cp, cp_cmd_t *cmd) {
  return 0;
}

int cp_start(void *cp) {
  return 0;
}

void cp_push_binarg(cp_arg_t *arg, void *buf, size_t len, int do_copy) {
}
