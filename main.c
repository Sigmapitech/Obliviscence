#include <dlfcn.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static const int MAGIC_BAD_STATUS = 77;

void resolve_symbol(void **funcp, const char *sym_name)
{
    if (*funcp == NULL)
        *funcp = dlsym(RTLD_NEXT, sym_name);
    if (*funcp == NULL)
        fprintf(stderr, "couldn't find symbol: [%s].", sym_name);
}

static
void *dispath_fork(pid_t pid, bool *in_fork, size_t request_size)
{
    static void *(*real)(size_t size) = NULL;
    int status;

    if (real == NULL)
        resolve_symbol((void **)&real, "malloc");
    if (pid == 0) {
        fprintf(stderr, "returning NULL in child...\n");
        *in_fork = true;
        return NULL;
    }
    wait(&status);
    status %= 255;
    fprintf(stderr, "child existed with status: %d\n", status);
    if (status == MAGIC_BAD_STATUS) {
        fprintf(stderr, "WAS NOT PROPERLY CHEKCED\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "returning real malloc...\n");
    return real(request_size);
}

void *malloc(size_t size)
{
    static bool in_fork = false;
    void *p;
    pid_t pid;

    if (in_fork) {
        fprintf(stderr, "\033[48;5;1m\033[38;5;232m"
            "CALL AFTER A BAD PREVIOUS MALLOC\033[0m\n");
        exit(MAGIC_BAD_STATUS);
    }
    fprintf(stderr, "\033[48;5;23m\033[38;5;232m"
        "malloc(size=%zu)\033[0m\n", size);
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    return dispath_fork(pid, &in_fork, size);
}
