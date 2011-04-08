#include <sys/inotify.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void usage(char *prog) {
  fprintf(stderr, "usage: %s [-v]\n", prog);
  exit(-1);
}

int verbose;

int main(int argc, char *argv[]) {
  int opt;
  while ( (opt = getopt(argc, argv, "v+") != -1)) {
    switch(opt) {
      case 'v': verbose++; break;
    }
  }
  if (optind < argc) usage(argv[0]);
}
