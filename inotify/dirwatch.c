#include <sys/inotify.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* start this program with a directory argument.
 * then go into that directory and create, remove,
 * rename, or read and write to files and watch
 * as inotify receives the event notifications.
 * Requires Linux 2.6.13 or higher */

int main(int argc, char *argv[]) {
  int fd, wd, mask, rc;
  char *dir, *name;

  if (argc != 2) {
    fprintf(stderr,"usage: %s <dir>\n", argv[0]);
    exit(-1);
  }

  dir = argv[1];

  if ( (fd = inotify_init()) == -1) {
    perror("inotify_init failed");
    exit(-1); 
  }

  mask = IN_ALL_EVENTS;
  if ( (wd = inotify_add_watch(fd, dir, mask)) == -1) {
    perror("inotify_add_watch failed");
    exit(-1); 
  }

  /* see inotify(7) as inotify_event has a trailing name
   * field allocated beyond the fixed structure; we must
   * allocate enough room for the kernel to populate it */
  struct inotify_event *event;
  size_t event_sz = sizeof(*event) + PATH_MAX;
  if ( (event = malloc(event_sz)) == NULL) {
    fprintf(stderr, "out of memory\n");
    exit(-1);
  }

  while ( (rc=read(fd,event,event_sz)) > 0) {
    name = (event->len ? event->name : dir);
    printf("%s\n", name);
    if (event->mask & IN_ACCESS) printf(" IN_ACCESS\n");
    if (event->mask & IN_ATTRIB) printf(" IN_ATTRIB\n");
    if (event->mask & IN_CLOSE_WRITE) printf(" IN_CLOSE_WRITE\n");
    if (event->mask & IN_CLOSE_NOWRITE) printf(" IN_CLOSE_NOWRITE\n");
    if (event->mask & IN_CREATE) printf(" IN_CREATE\n");
    if (event->mask & IN_DELETE) printf(" IN_DELETE\n");
    if (event->mask & IN_DELETE_SELF) printf(" IN_DELETE_SELF\n");
    if (event->mask & IN_MODIFY) printf(" IN_MODIFY\n");
    if (event->mask & IN_MOVE_SELF) printf(" IN_MOVE_SELF\n");
    if (event->mask & IN_MODIFY) printf(" IN_MODIFY\n");
    if (event->mask & IN_MOVE_SELF) printf(" IN_MOVE_SELF\n");
    if (event->mask & IN_MOVED_FROM) printf(" IN_MOVED_FROM\n");
    if (event->mask & IN_MOVED_TO) printf(" IN_MOVED_TO\n");
    if (event->mask & IN_OPEN) printf(" IN_OPEN\n");
    printf("\n");
  }

  close(fd);
}
