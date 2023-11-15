#include "kernel/types.h"
#include "user.h"


void getPrimes(int fd) {
    int my_num = 0;
    int forked = 0;
    int pass_num = 0;
    int pipes[2];
    while (1) {
        int read_bytes = read(fd, &pass_num, 4);

        // left pipeline has no more number
        if (read_bytes == 0) {
            // close child no more number
            close(pipes[1]);
            // wait children
            if (forked) {
                int child;
                wait(&child);
            }
            exit(0);
        }

        // init read
        if (my_num == 0) {
            my_num = pass_num;
            printf("prime %d\n", my_num);
        }

        // if no my number pass alone
        if (pass_num % my_num != 0) {
            if (!forked) {
                pipe(pipes);
                forked = 1;
                int ret = fork();

                if (ret == 0) {
                    // i am child
                    close(pipes[1]);
                    close(fd);
                    getPrimes(pipes[0]);
                } else {
                    // i am the parent
                    close(pipes[0]);
                }
            }
            write(pipes[1], &pass_num, 4);
        }
    }
}


int main(int argc, char *argv[]) {
    int pipes[2];
    pipe(pipes);
    for (int i = 2; i < 35; ++i) {
        write(pipes[1], &i, 4);
    }
    close(pipes[1]);
    getPrimes(pipes[0]);
    exit(0);
}