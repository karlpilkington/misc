#include <sys/inotify.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>

void usage(char *prog) {
  fprintf(stderr, "usage: %s [-v] [-n <nthreads>]\n", prog);
  exit(-1);
}

int verbose;
int nthreads=2;
char *dir="/tmp";

union {
  struct inotify_event ev;
  char buf[sizeof(struct inotify_event) + PATH_MAX];
} eb;

char *get_file(int fd, void **nx) {
  struct inotify_event *ev;
  static int rc=0;
  size_t sz;

  if (*nx) ev = *nx;
  else {
    rc = read(fd,&eb,sizeof(eb));
    if (rc < 0) return NULL;
    ev = &eb.ev;
  }

  sz = sizeof(*ev) + ev->len;
  rc -= sz;
  *nx = (rc > 0) ? ((char*)ev + sz) : NULL;
  return ev->len ? ev->name : dir;
}

int main(int argc, char *argv[]) {
  int opt, fd, wd;
  void *nx=NULL;
  char *f;

  while ( (opt = getopt(argc, argv, "v+n:d:") != -1)) {
    switch(opt) {
      case 'v': verbose++; break;
      case 'n': nthreads=atoi(optarg); break;
      case 'd': dir=strdup(optarg); break;
    }
  }
  if (optind < argc) usage(argv[0]);

  fd = inotify_init();
  wd = inotify_add_watch(fd,dir,IN_CLOSE);
  while ( (f=get_file(fd,&nx))) {
    //printf("%s (nx %p)\n", f, nx);
  }

  close(fd);
}
