#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#ifdef __DEBUGMSG
#define DEBUGMSG(fmt, args...) fprintf(stderr, "[DEBUGMSG]{%s:%d:%s()}: " fmt, \
__FILE__, __LINE__, __func__, ##args)
#else
#define DEBUGMSG(fmt, args...)
#endif

/* Functions */
/* Ping Process [I] */
void recv_signal(){
    exit(0);
}

/* Main function */
int main(int argc, char *argv[]){

    sigset_t sigset; // Block SIGUSR1
    sigset_t oldset; // Prev sigset
    struct sigaction act; // 0: SIGUSR1 / 1: SIGUSR2

    /* Set a sigset blocking SIGUSR1 */
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    pid_t pid;
    if(!(pid = fork())){ // Child
        
        /* Setting Handler */
        act.sa_handler = recv_signal;
        act.sa_flags = 0;

        sigaction(SIGUSR1, &act, NULL);
        // sigprocmask(SIG_BLOCK, &sigset, &oldset); // Block SIGUSR1
        pause();
        
    } else { // Parent
        /* Setting Handler */
        // sigprocmask(SIG_BLOCK, &sigset, &oldset); // Block SIGUSR1, SIGUSR2
        sleep(5);
        kill(pid, SIGUSR1);
    }

    return 0;
}