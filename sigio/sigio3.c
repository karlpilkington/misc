#define _GNU_SOURCE
#include <sys/inotify.h>
#include <sys/types.h>
#include <signal.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <setjmp.h>

/* start this program with a directory argument.
 * then go into that directory and create, remove,
 * rename, or read and write to files and watch
 * as inotify receives the event notifications.
 * Requires Linux 2.6.13 or higher */

sigjmp_buf jmp;
/* signals that we'll unblock during sigsuspend */
int sigs[] = {0,SIGHUP,SIGCHLD,SIGTERM,SIGINT,SIGQUIT,SIGALRM};

void read_events(int fd, struct inotify_event *eb, size_t eb_sz, char *dir) {
  struct inotify_event *ev, *nx;
  char *name;
  size_t sz;
  int rc;
  /* one read will produce one or more event structures */
  while ( (rc=read(fd,eb,eb_sz)) > 0) {
    for(ev = eb; rc > 0; ev = nx) {

      sz = sizeof(*ev) + ev->len;
      nx = (struct inotify_event*)((char*)ev + sz);
      rc -= sz;

      name = (ev->len ? ev->name : dir);
      printf("%s ", name);
      if (ev->mask & IN_ACCESS) printf(" IN_ACCESS");
      if (ev->mask & IN_MODIFY) printf(" IN_MODIFY");
      if (ev->mask & IN_ATTRIB) printf(" IN_ATTRIB");
      if (ev->mask & IN_CLOSE_WRITE) printf(" IN_CLOSE_WRITE");
      if (ev->mask & IN_CLOSE_NOWRITE) printf(" IN_CLOSE_NOWRITE");
      if (ev->mask & IN_OPEN) printf(" IN_OPEN");
      if (ev->mask & IN_MOVED_FROM) printf(" IN_MOVED_FROM");
      if (ev->mask & IN_MOVED_TO) printf(" IN_MOVED_TO");
      if (ev->mask & IN_CREATE) printf(" IN_CREATE");
      if (ev->mask & IN_DELETE) printf(" IN_DELETE");
      if (ev->mask & IN_DELETE_SELF) printf(" IN_DELETE_SELF");
      if (ev->mask & IN_MOVE_SELF) printf(" IN_MOVE_SELF");
      printf("\n");
    }
  }
}

volatile int sigfd; /* just for sanity checking - should match fd */
void sighandler(int signo, siginfo_t *info, void *ucontext) {
  sigfd = info->si_fd; /* see sigaction(2), inotify(7) */
  siglongjmp(jmp,signo);
}

int main(int argc, char *argv[]) {
  int fd, wd, mask, rc, n;
  char *dir;
  sigs[0] = SIGRTMIN+0;

  if (argc != 2) {
    fprintf(stderr,"usage: %s <dir>\n", argv[0]);
    exit(-1);
  }

  dir = argv[1];

  if ( (fd = inotify_init()) == -1) {
    perror("inotify_init failed");
    exit(-1); 
  }

  /* set up signal I/O notification for fd; see fcntl(2) */
  int fl = fcntl(fd, F_GETFL);
  fl |= O_ASYNC;
  if (fcntl(fd, F_SETFL, fl) == -1) {
    perror("fcntl O_ASYNC failed");
    exit(-1); 
  }

  /* now we use fcntl(fd,F_SETSIG,SIGRTMIN) etc to change
   * the signal we want when fd is ready, instead of SIGIO. Why?
   * Because these other signals will receive "additional info"
   * (in particular, the fd that is ready) if that signal handler
   * is installed with SA_SIGINFO. But NOTE that only if the
   * chosen signal number is an RT signal (>= SIGRTMIN) will
   * these signal events be QUEUED; this is necessary to get
   * differentiable notifications for _different_ fd's all using
   * one signal. */
  if (fcntl(fd, F_SETSIG, sigs[0]) == -1) {
    perror("fcntl F_SETOWN failed");
    exit(-1); 
  }
   /* if this were a multi-threaded program, the previous call
    * also sets the thread recipient; see fcntl(2) */
  if (fcntl(fd, F_SETOWN, getpid()) == -1) {
    perror("fcntl F_SETOWN failed");
    exit(-1); 
  }

  /* finish setting up the inotify watch now that fd signals are set */
  mask = IN_ALL_EVENTS;
  if ( (wd = inotify_add_watch(fd, dir, mask)) == -1) {
    perror("inotify_add_watch failed");
    exit(-1); 
  }

  /* see inotify(7) as inotify_event has a trailing name
   * field allocated beyond the fixed structure; we must
   * allocate enough room for the kernel to populate it */
  struct inotify_event *eb;
  size_t eb_sz = sizeof(*eb) + PATH_MAX;
  if ( (eb = malloc(eb_sz)) == NULL) {
    fprintf(stderr, "out of memory\n");
    exit(-1);
  }

  /* block all signals. we stay blocked always except in sugsuspend */
  sigset_t all;
  sigfillset(&all);
  sigprocmask(SIG_SETMASK,&all,NULL);

  /* a smaller set of signals we'll block during sigsuspend */
  sigset_t ss;
  sigfillset(&ss);
  for(n=0; n < sizeof(sigs)/sizeof(*sigs); n++) sigdelset(&ss, sigs[n]);

  /* establish handlers for signals that'll be unblocked in sigsuspend */
  struct sigaction sa;
  sa.sa_sigaction=sighandler; 
  sa.sa_flags=SA_SIGINFO;
  sigfillset(&sa.sa_mask);
  for(n=0; n < sizeof(sigs)/sizeof(*sigs); n++) sigaction(sigs[n], &sa, NULL);

  /* here is a special line. we'll come back here whenever a signal happens */
  int signo = sigsetjmp(jmp,1);

  if (signo == 0) { 
    /* initial setup, no signal yet */
  } else if (signo == sigs[0]) {
    printf("siginfo indicated fd %d (expected %d)\n", sigfd, fd);
    read_events(fd,eb,eb_sz,dir);
  } else {
    printf("got signal %d\n", signo);
    goto done;
  }

  /* wait for signals */
  sigsuspend(&ss);

  /* the only way we get past this point
   * is from the "goto done" above, because
   * sigsuspend waits for signals, and when
   * one arrives we longjmp back to sigsetjmp! */

done:
  close(fd);
}
