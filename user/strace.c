#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int
main(int argc, char **argv)
{
    int To_trace = atoi(argv[1]); //mask
    trace(To_trace);
    exec(argv[2], &argv[2]);
    printf("error strace did not exec");
    exit(1);
}