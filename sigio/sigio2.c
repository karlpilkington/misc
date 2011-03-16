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
#include <string.h>

/* start this program with a directory argument.
 * then go into that directory and create, remove,
 * rename, or read and write to files and watch
 * as inotify receives the event notifications.
 * Requires Linux 2.6.13 or higher */

sigjmp_buf jmp;
/* signals that we'll unblock during sigsuspend; first is placeholder
 * because SIGRTMIN isn't necessarily a compile-time define.  */
int sigs[] = {0,SIGHUP,SIGCHLD,SIGTERM,SIGINT,SIGQUIT,SIGALRM};

volatile int sigfd; /* will indicate 'which' fd is ready */
void sighandler(int signo, siginfo_t *info, void *ucontext) {
  sigfd = info->si_fd;  /* also see info->si_code in sigaction(2) */
  siglongjmp(jmp,signo);
}

void spawn_child(int fd[2], char *greeting, char *goodbye) {
  if (fork() != 0) return;  /* parent */
  close(fd[0]);
  sleep(1); write(fd[1],greeting,strlen(greeting));
  sleep(1); write(fd[1],goodbye, strlen(goodbye));
  close(fd[1]);
  exit(0);
}

int main(int argc, char *argv[]) {
  int fl, fa[2], fb[2], rc, n, closed=0;
  char buf[100];

  sigs[0] = SIGRTMIN+0;  /* we'll choose this RT signal for I/O readiness */

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
  sa.sa_sigaction=sighandler;  /* instead of sa_handler; takes extra args */
  sa.sa_flags=SA_SIGINFO;      /* requests extra info in handler */
  sigfillset(&sa.sa_mask);
  for(n=0; n < sizeof(sigs)/sizeof(*sigs); n++) sigaction(sigs[n], &sa, NULL);

  pipe(fa); /* to read from child1 */
  pipe(fb); /* to read from child2 */

  /* request signal I/O, SIGRTMIN instead of SIGIO, sent to us on fd ready */
  fl = fcntl(fa[0], F_GETFL); fcntl(fa[0], F_SETFL, fl|O_ASYNC|O_NONBLOCK);
  fl = fcntl(fb[0], F_GETFL); fcntl(fb[0], F_SETFL, fl|O_ASYNC|O_NONBLOCK);
  fcntl(fa[0], F_SETSIG, sigs[0]); fcntl(fa[0], F_SETOWN, getpid());
  fcntl(fb[0], F_SETSIG, sigs[0]); fcntl(fb[0], F_SETOWN, getpid());

  spawn_child(fa, "hello\n", "world\n");
  spawn_child(fb, "enjoy\n", "cocoa\n");

  /* here is a special line. we'll come back here whenever a signal happens */
  int signo = sigsetjmp(jmp,1);

  switch(signo) {
    case 0: /* initial setup, no signal yet */
      break;
    case SIGCHLD:
      printf("sigchld\n"); /* note sigchld can happen before final SIGRT */
      break;
    default: /* want "case SIGRTMIN" but its not compile time constant */
      if (signo == sigs[0]) { /* SIGRTMIN */
        rc = read(sigfd, buf, sizeof(buf));
        if (rc > 0) printf("read(%d): %.*s", sigfd, rc, buf);
        else if (rc == 0) {close(sigfd); if (++closed==2) goto done;}
        else if (rc == -1) {perror("read error"); goto done;}
      } else {
        printf("got signal %d\n", signo);
        goto done;
      }
      
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
