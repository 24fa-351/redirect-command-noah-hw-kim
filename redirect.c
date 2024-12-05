#include "redirect.h"
// #include <sys/types>

/*
    // ./redir - "cmd param parm" -
    // 1) break argv[2] into words
    // 2) find the aboslute path of the command
*/

void add_character_to_string(char* str, char c) {
    int len = strlen(str);
    str[len] = c;
    str[len + 1] = '\0';
}

// splits string by spaces; adds a NULL int to the array after the last word
void split_cmd(char* cmd, char* words[], char delimiter) {
    int word_ct = 0;
    char* next_char = cmd;
    char current_word[MAX_LENGTH];
    strcpy(current_word, "");

    while (*next_char != '\0') {
        if (*next_char == delimiter) {
            words[word_ct++] = strdup(current_word);
            strcpy(current_word, "");
        } else {
            add_character_to_string(current_word, *next_char);
        }
        ++next_char;
    }
    words[word_ct++] = strdup(current_word);
    words[word_ct] = NULL;
}

char* find_absolute_path(char* cmd) {
    printf("finding absolute path for %s\n", cmd);

    char* path_env = getenv("PATH");
    if (path_env == NULL) {
        printf("no path\n");
        return NULL;
    }

    char* path_env_copy = strdup(path_env);
    if (path_env_copy == NULL) {
        printf("strdup failed\n");
        return NULL;
    }

    char* token = strtok(path_env_copy, ":");
    while (token != NULL) {
        char full_path[MAX_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", token, cmd);

        if (access(full_path, X_OK) == 0) {
            free(path_env_copy);
            return strdup(full_path);
        }
        token = strtok(NULL, ":");
    }
    free(path_env_copy);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <input> <cmd> <output> \n", argv[0]);
        return 1;
    }

    char* input_file = argv[1];
    char* output_file = argv[3];
    bool has_input_file = true;
    bool has_output_file = true;

    if (strcmp(input_file, "-") == 0) {
        has_input_file = false;
    }

    if (strcmp(output_file, "-") == 0) {
        has_output_file = false;
    }

    char* cmd[MAX_LENGTH];
    split_cmd(argv[2], cmd, ' ');

    if (cmd[0] == NULL) {
        fprintf(stderr, "No command found\n");
        return 1;
    }

    for (int idx = 0; cmd[idx] != NULL; idx++) {
        printf("command[%d] = %s\n", idx, cmd[idx]);
    }

    char* abs_path = find_absolute_path(cmd[0]);
    if (abs_path == NULL) {
        printf("no absolute path found\n");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        if (has_input_file) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in < -1) {
                fprintf(stderr, "Failed to open %s\n", input_file);
                _exit(1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        if (has_output_file) {
            int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0) {
                fprintf(stderr, "Failed to open %s\n", output_file);
                _exit(1);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        execve(abs_path, cmd, NULL);
        fprintf(stderr, "Failed to execute %s\n", cmd[0]);
        _exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        fprintf(stderr, "Failed to fork\n");
        return 1;
    }

    free(abs_path);
    for (int idx = 0; cmd[idx] != NULL; idx++) {
        free(cmd[idx]);
    }

    return 0;
}