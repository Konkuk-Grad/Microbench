#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define _STR(x) #x
#define STR(x) _STR(x)
#define MAX_PATH 256

const char *find_tracefs(void)
{
       static char tracefs[MAX_PATH+1];
       static int tracefs_found;
       char type[100];
       FILE *fp;

       if (tracefs_found)
               return tracefs;

       if ((fp = fopen("/proc/mounts","r")) == NULL) {
               perror("/proc/mounts");
               return NULL;
       }

       while (fscanf(fp, "%*s %"
                     STR(MAX_PATH)
                     "s %99s %*s %*d %*d\n",
                     tracefs, type) == 2) {
               if (strcmp(type, "tracefs") == 0)
                       break;
       }
       fclose(fp);

       if (strcmp(type, "tracefs") != 0) {
               fprintf(stderr, "tracefs not mounted");
               return NULL;
       }

//        strcat(tracefs, "/tracing");
       tracefs_found = 1;

       return tracefs;
}

const char *tracing_file(const char *file_name)
{
       static char trace_file[MAX_PATH+1];
       snprintf(trace_file, MAX_PATH, "%s/%s", find_tracefs(), file_name);
       return trace_file;
}

int main (int argc, char **argv)
{
        if (argc < 1){
            fprintf(stderr, "[argc] : error");
            exit(-1);
        }
        if (fork() > 0) {
                int fd, ffd;
                char line[64];
                int s;

                ffd = open(tracing_file("current_tracer"), O_WRONLY);
                if (ffd < 0)
                        exit(-1);
                write(ffd, "nop", 3);

                fd = open(tracing_file("set_ftrace_pid"), O_WRONLY);
                s = sprintf(line, "%d\n", getpid());
                write(fd, line, s);

                write(ffd, "function_graph", 14);

                close(fd);
                close(ffd);

                execv(argv[1], argv+1);
        }

        return 0;
}