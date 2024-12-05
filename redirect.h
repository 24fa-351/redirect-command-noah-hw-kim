#ifndef REDIRECT_H
#define REDIRECT_H

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LENGTH 1000

void add_character_to_string(char* str, char c);
void split_cmd(char* cmd, char* words[], char delimiter);
char* find_absolute_path(char* cmd);

#endif