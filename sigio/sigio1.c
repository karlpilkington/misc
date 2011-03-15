#define _GNU_SOURCE
#include <sys/types.h>
#include <signal.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <setjmp.h>

sigjmp_buf jmp;
/* signals that we'll unblock during sigsuspend */
int sigs[] = {0,SIGHUP,SIGCHLD,SIGTERM,SIGINT,SIGQUIT,SIGALRM};

volatile int sigfd; /* just for sanity checking - should match fd */
void sighandler(int signo, siginfo_t *info, void *ucontext) {
  sigfd = info->si_fd; /* see sigaction(2), inotify(7) */
  siglongjmp(jmp,signo);
}

int main(int argc, char *argv[]) {
  int fd=0, rc, n;
  char buf[10];
  sigs[0] = SIGRTMIN+0;

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
    if ( (rc = read(fd, buf, sizeof(buf))) <= 0) goto done;
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
  return 0;
}
