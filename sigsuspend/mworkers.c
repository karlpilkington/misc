
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <sys/types.h>

/*
 *
 */

int wr;    /* workers running */
int wn=2;  /* workers needed */

sigjmp_buf jmp;
/* signals that we'll unblock during sigsuspend */
int sigs[] = {SIGHUP,SIGCHLD,SIGTERM,SIGALRM,SIGUSR1};

typedef struct {
  pid_t pid;
  time_t start;
} worker_t;

worker_t *workers;

void sighandler(int signo) {
  siglongjmp(jmp,signo);
}
 
void worker(int w) {
  pid_t pid;
  int n;

  if ( (pid = fork()) == -1) {
    printf("fork error\n"); 
    exit(-1); 
  }
  if (pid > 0) { /* parent. */
    printf("worker %d started\n", (int)pid);
    /* record worker */
    workers[w].pid = pid;
    workers[w].start = time(NULL);
    return;
  } 

  /* child here */
  /* setproctitle() or prctl(PR_SET_NAME) */

  /* restore default signal handlers, then unblock all signasl */
  for(n=0; n < sizeof(sigs)/sizeof(*sigs); n++) signal(sigs[n],SIG_DFL);
  sigset_t all;
  sigemptyset(&all);
  sigprocmask(SIG_SETMASK,&all,NULL);

  /* do something to simulate real work */
  int maxsleep = 30;
  int awhile = maxsleep*rand()/RAND_MAX*1.0;
  sleep(awhile);
  exit(0);
}

int main(int argc, char *argv[]) {
    pid_t pid;
    int n,es;

    if ( (workers = malloc(sizeof(worker_t)*wn)) == NULL) 
        exit(-1);

    /* block all signals. we stay blocked always except in sugsuspend */
    sigset_t all;
    sigfillset(&all);
    sigprocmask(SIG_SETMASK,&all,NULL);

    /* a smaller set of signals we'll block during sigsuspend */
    sigset_t ss;
    sigfillset(&ss);
    for(n=0; n < sizeof(sigs)/sizeof(*sigs); n++) sigdelset(&ss, sigs[n]);

    /* establish handlers for signals that'll be unblocked in sigusupend */
    struct sigaction sa;
    sa.sa_handler=sighandler; 
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);
    for(n=0; n < sizeof(sigs)/sizeof(*sigs); n++) sigaction(sigs[n], &sa, NULL);

    /* here is a special line. we'll come back here whenever a signal ahppens */
    int signo = sigsetjmp(jmp,1);

    switch(signo) {
        case 0:   /* not a signal yet, first time setup */
          while(wr < wn) worker(wr++); /* bring up wn workers */
          break;
        case SIGCHLD:
          /* loop over children that have exited */
          while( (pid = waitpid(-1,&es,WNOHANG)) > 0) {
              for(n=0; n < wr; n++) if (workers[n].pid==pid) break;
              assert(n != wr);
              int elapsed = time(NULL) - workers[n].start;
              printf("pid %d exited after %d seconds: ", (int)pid, elapsed);
              if (WIFEXITED(es)) printf("exit status %d\n", (int)WEXITSTATUS(es));
              else if (WIFSIGNALED(es)) printf("signal %d\n", (int)WTERMSIG(es));
              if (n < wr-1) memmove(&workers[n],&workers[n+1],sizeof(worker_t)*(wr-1-n));
              wr--;
          }
          while(wr < wn) worker(wr++); /* bring up wn workers */
          break;
        case SIGALRM:
          while(wr < wn) worker(wr++); /* bring up wn workers */
          break;
        default:
          printf("got signal %d\n", signo);
          goto done;
          break;
    }
    /* wait for signals */
    sigsuspend(&ss);

    /* the only way we get past this point
     * is from the "goto done" above, because
     * sigsuspend waits for signals, and when
     * one arrives we longjmp back to sigsetjmp! */

done:
  /* reap any running workers */
  while (wr) {
    n = --wr;
    printf("terminating pid %d\n", workers[n]);
    kill(workers[n].pid, SIGTERM);
    waitpid(workers[n].pid, NULL, 0);
  }
}

