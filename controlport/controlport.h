#ifndef CONTROL_PORT_H
#define CONTROL_PORT_H

#include <stdlib.h>

typedef struct {
  int     argc;      /* conventional argc/argv args */
  char  **argv;
  int     bin_argc;  /* binary buffer arguments */
  size_t *bin_lenv;
  char  **bin_argv;
} cp_arg_t;

typedef int (cp_cmd_f)(cp_arg_t *arg, cp_arg_t *out);

typedef struct {
  char *name;
  cp_cmd_f *cmdf;
} cp_cmd_t;

void *cp_init(char *path, cp_cmd_t *cmds, int timeout);
int cp_add_cmd(void*, cp_cmd_t *cmd);
int cp_start(void*);
void cp_push_binarg(cp_arg_t *arg, void *buf, size_t len, int do_copy);

#endif 
