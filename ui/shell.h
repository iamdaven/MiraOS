#pragma once

#include <stdint.h>
#include <stdbool.h>

#define SHELL_MAX_CMD_LEN 4096
#define SHELL_MAX_ARGS 256
#define SHELL_MAX_VARS 1024
#define SHELL_MAX_ALIASES 256
#define SHELL_MAX_HISTORY 1024
#define SHELL_MAX_SCRIPTS 64
#define SHELL_MAX_JOBS 32
#define SHELL_MAX_PIPELINE 16
#define SHELL_MAX_REDIRECTS 8
#define SHELL_SCRIPT_MAGIC 0x4D455348

typedef enum {
    TOKEN_WORD,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_BACKGROUND,
    TOKEN_SEMICOLON,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF,
    TOKEN_ERROR
} token_type_t;

typedef enum {
    NODE_COMMAND,
    NODE_PIPELINE,
    NODE_REDIRECT,
    NODE_BACKGROUND,
    NODE_SEQUENCE,
    NODE_AND,
    NODE_OR,
    NODE_SUBSHELL,
    NODE_FOR,
    NODE_WHILE,
    NODE_IF,
    NODE_FUNCTION,
    NODE_VAR_ASSIGN,
    NODE_EXPORT,
    NODE_ALIAS
} ast_node_type_t;

typedef enum {
    VAR_TYPE_STRING,
    VAR_TYPE_NUMBER,
    VAR_TYPE_ARRAY,
    VAR_TYPE_ASSOC
} var_type_t;

typedef struct token {
    token_type_t type;
    char *value;
    uint32_t line;
    uint32_t column;
    struct token *next;
} token_t;

typedef struct ast_node {
    ast_node_type_t type;
    struct ast_node **children;
    uint32_t child_count;
    char *value;
    var_type_t var_type;
    union {
        struct {
            char **args;
            uint32_t arg_count;
            char *input_redirect;
            char *output_redirect;
            char *error_redirect;
            bool append_output;
            bool append_error;
            bool background;
        } cmd;
        struct {
            char *var_name;
            char *var_value;
            bool exported;
        } assign;
        struct {
            char *name;
            struct ast_node **params;
            uint32_t param_count;
            struct ast_node *body;
        } func;
    } data;
} ast_node_t;

typedef struct {
    char name[64];
    char value[256];
    var_type_t type;
    bool exported;
    bool readonly;
} shell_var_t;

typedef struct {
    char name[64];
    char value[256];
} shell_alias_t;

typedef struct {
    uint32_t id;
    char name[64];
    ast_node_t *ast;
    shell_var_t *local_vars;
    uint32_t local_var_count;
    uint32_t stack_size;
    bool running;
    bool suspended;
    int exit_status;
} shell_function_t;

typedef struct {
    uint32_t job_id;
    char command[SHELL_MAX_CMD_LEN];
    uint32_t pid;
    bool running;
    bool stopped;
    int exit_status;
} shell_job_t;

typedef struct {
    char *script_data;
    uint32_t script_size;
    char filename[256];
    uint32_t line_number;
    token_t *tokens;
    ast_node_t *ast;
    shell_var_t *variables;
    uint32_t var_count;
    shell_alias_t *aliases;
    uint32_t alias_count;
    shell_function_t *functions;
    uint32_t function_count;
    shell_job_t *jobs;
    uint32_t job_count;
    char *history[SHELL_MAX_HISTORY];
    uint32_t history_count;
    uint32_t history_index;
    bool interactive;
    bool exit_requested;
    int last_exit_status;
    char current_dir[256];
    char home_dir[256];
    char path[1024];
    uint32_t uid;
    uint32_t gid;
    uint64_t start_time;
    uint64_t command_count;
    bool debug_mode;
    bool verbose_mode;
    int script_fd;
    int input_fd;
    int output_fd;
    int error_fd;
} shell_ctx_t;

typedef struct {
    char *name;
    int (*handler)(shell_ctx_t *ctx, ast_node_t *node);
    char *description;
    uint32_t flags;
} builtin_cmd_t;

typedef enum {
    REDIRECT_NONE,
    REDIRECT_INPUT,
    REDIRECT_OUTPUT,
    REDIRECT_APPEND,
    REDIRECT_ERROR,
    REDIRECT_ERROR_APPEND,
    REDIRECT_DUP
} redirect_type_t;

typedef struct {
    redirect_type_t type;
    int fd;
    char *filename;
    int target_fd;
} redirect_t;

typedef struct {
    ast_node_t *command;
    redirect_t redirects[SHELL_MAX_REDIRECTS];
    uint32_t redirect_count;
    int input_fd;
    int output_fd;
    int error_fd;
    bool background;
} pipeline_stage_t;

typedef struct {
    pipeline_stage_t stages[SHELL_MAX_PIPELINE];
    uint32_t stage_count;
    int pipe_fds[2];
    bool background;
} pipeline_t;

void shell_init(void);
void shell_tick(void);
void shell_render(void);
bool shell_dirty(void);
void shell_clear_dirty(void);
void shell_execute_command(const char *cmd);
void shell_run_script(const char *path);
void shell_set_variable(const char *name, const char *value);
const char *shell_get_variable(const char *name);
void shell_export_variable(const char *name);
void shell_add_alias(const char *name, const char *value);
const char *shell_resolve_alias(const char *name);
void shell_register_builtin(const char *name, int (*handler)(shell_ctx_t *, ast_node_t *), const char *desc);
int shell_evaluate_expression(shell_ctx_t *ctx, ast_node_t *node);
void shell_print_error(const char *msg);
void shell_print_warning(const char *msg);
void shell_print_debug(const char *msg);
void shell_add_history(const char *cmd);
const char *shell_get_history(uint32_t index);
void shell_job_add(shell_job_t *job);
void shell_job_remove(uint32_t job_id);
shell_job_t *shell_job_get(uint32_t job_id);
void shell_job_list(void);
void shell_job_foreground(uint32_t job_id);
void shell_job_background(uint32_t job_id);
void shell_job_kill(uint32_t job_id);
void shell_function_register(const char *name, ast_node_t *params, ast_node_t *body);
shell_function_t *shell_function_get(const char *name);
void shell_signal_handler(int signal);
void shell_cleanup(void);