//
// Created by neit on 10.03.17.
//

#include <stdio.h>
#include <stdlib.h>

int is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\n';
}

int start_with(const char *line, const char *pre)
{
    while(*pre) {
        if (*pre++ != *line++) {
            return 0;
        }
    }
    return 1;
}

int read_conf(const char *file_name, char *remote)
{
    FILE *f;
    char *line, *line_start;
    size_t len = 0;
    ssize_t read;

    if ((f = fopen(file_name, "r")) == NULL) {
        exit(-1);
    }

    while((read = getline(&line, &len, f)) != -1) {

        for (line_start = line; *line_start != 0; line_start++) {
            if (!is_space(*line_start)) {
                break;
            }
        }

        if (*line == '#') {
            continue;
        }

        if (!start_with(line, "nameserver")) {
            continue;
        }

        while(*line_start != 0) {
            if (*line_start >= '0' && *line_start <= '9' || *line_start == '.') {
                *remote = *line_start;
                ++remote;
            }
            ++line_start;
        }

        *remote = '\0';
    }

    if (line) {
        free(line);
    }

    return 0;
}