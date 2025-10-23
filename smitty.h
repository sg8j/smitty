#ifndef SMITTY_H
#define SMITTY_H

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdarg.h>
#include "hide.h"
#include "hooks.h"

#define SMITTY_VERSION "1.0.0"
#define SMITTY_MAGIC 0xDEADBEEF

typedef struct {
    unsigned int magic;
    char version[16];
    int initialized;
} smitty_context_t;

extern smitty_context_t g_smitty_ctx;

void smitty_init(void);
void smitty_cleanup(void);
int smitty_is_initialized(void);

#endif
