#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TextBuffer* buffer_create(void) {
    TextBuffer *buf = malloc(sizeof(TextBuffer));
    if (!buf) {
        return NULL;
    }
    buf->num_lines = 0;
    for (int i = 0; i < MAX_LINES; i++) {
        buf->lines[i] = malloc(MAX_LINE_LENGTH);
        if (!buf->lines[i]) {
            // Free previously allocated lines on failure
            for (int j = 0; j < i; j++) {
                free(buf->lines[j]);
            }
            free(buf);
            return NULL;
        }
        buf->lines[i][0] = '\0';
        buf->bold_state[i] = 0;
    }
    buf->num_lines = 1; // Always start with at least 1 empty line
    return buf;
}

void buffer_free(TextBuffer *buf) {
    if (!buf) return;
    for (int i = 0; i < MAX_LINES; i++) {
        free(buf->lines[i]);
    }
    free(buf);
}

int buffer_load(TextBuffer *buf, const char *filename) {
    if (!buf || !filename) return 0;
    FILE *file = fopen(filename, "r");
    if (!file) {
        // File doesn't exist or can't be opened, start with a clean single empty line
        buf->lines[0][0] = '\0';
        buf->num_lines = 1;
        buf->bold_state[0] = 0;
        return 1;
    }

    int count = 0;
    char temp[MAX_LINE_LENGTH];
    while (fgets(temp, MAX_LINE_LENGTH, file) && count < MAX_LINES) {
        // Strip trailing newline character if present
        temp[strcspn(temp, "\n")] = '\0';
        strncpy(buf->lines[count], temp, MAX_LINE_LENGTH - 1);
        buf->lines[count][MAX_LINE_LENGTH - 1] = '\0';
        buf->bold_state[count] = 0;
        count++;
    }
    fclose(file);

    if (count == 0) {
        buf->lines[0][0] = '\0';
        buf->bold_state[0] = 0;
        buf->num_lines = 1;
    } else {
        buf->num_lines = count;
    }
    return 1;
}

int buffer_save(const TextBuffer *buf, const char *filename) {
    if (!buf || !filename) return 0;
    FILE *file = fopen(filename, "w");
    if (!file) return 0;

    for (int i = 0; i < buf->num_lines; i++) {
        fputs(buf->lines[i], file);
        fputc('\n', file);
    }
    fclose(file);
    return 1;
}

int buffer_insert_char(TextBuffer *buf, int line, int col, char ch) {
    if (!buf || line < 0 || line >= buf->num_lines) return 0;
    int len = strlen(buf->lines[line]);
    if (len >= MAX_LINE_LENGTH - 1 || col < 0 || col > len) return 0;

    for (int i = len; i >= col; i--) {
        buf->lines[line][i + 1] = buf->lines[line][i];
    }
    buf->lines[line][col] = ch;
    return 1;
}

int buffer_insert_newline(TextBuffer *buf, int *line, int *col) {
    if (!buf || !line || !col) return 0;
    int curr_line = *line;
    int curr_col = *col;

    if (buf->num_lines >= MAX_LINES) return 0;

    // Shift lines down to make room for the split/new line
    for (int i = buf->num_lines; i > curr_line + 1; i--) {
        strcpy(buf->lines[i], buf->lines[i - 1]);
        buf->bold_state[i] = buf->bold_state[i - 1];
    }

    // Split the current line content
    strcpy(buf->lines[curr_line + 1], &buf->lines[curr_line][curr_col]);
    buf->lines[curr_line][curr_col] = '\0';
    buf->bold_state[curr_line + 1] = 0; // New lines default to non-bold

    buf->num_lines++;
    (*line)++;
    *col = 0;
    return 1;
}

int buffer_delete_backspace(TextBuffer *buf, int *line, int *col) {
    if (!buf || !line || !col) return 0;
    int curr_line = *line;
    int curr_col = *col;

    if (curr_col > 0) {
        // Backspace within a line
        int len = strlen(buf->lines[curr_line]);
        if (curr_col > len) return 0;
        for (int i = curr_col - 1; i < len; i++) {
            buf->lines[curr_line][i] = buf->lines[curr_line][i + 1];
        }
        (*col)--;
        return 1;
    } else if (curr_line > 0 && curr_col == 0) {
        // Backspace at the beginning of a line (merge with previous line)
        int prev_len = strlen(buf->lines[curr_line - 1]);
        int curr_len = strlen(buf->lines[curr_line]);

        // Check if combined length fits in one line
        if (prev_len + curr_len < MAX_LINE_LENGTH) {
            strcat(buf->lines[curr_line - 1], buf->lines[curr_line]);
            // Shift remaining lines up
            for (int i = curr_line; i < buf->num_lines - 1; i++) {
                strcpy(buf->lines[i], buf->lines[i + 1]);
                buf->bold_state[i] = buf->bold_state[i + 1];
            }
            buf->num_lines--;
            (*line)--;
            *col = prev_len;
            return 1;
        }
    }
    return 0;
}

int buffer_delete_char(TextBuffer *buf, int line, int col) {
    if (!buf || line < 0 || line >= buf->num_lines) return 0;
    int len = strlen(buf->lines[line]);

    if (col < len) {
        // Delete in the middle of a line
        for (int i = col; i < len; i++) {
            buf->lines[line][i] = buf->lines[line][i + 1];
        }
        return 1;
    } else if (line < buf->num_lines - 1) {
        // Delete at the end of a line (merge next line into current)
        int next_len = strlen(buf->lines[line + 1]);
        if (len + next_len < MAX_LINE_LENGTH) {
            strcat(buf->lines[line], buf->lines[line + 1]);
            // Shift remaining lines up
            for (int i = line + 1; i < buf->num_lines - 1; i++) {
                strcpy(buf->lines[i], buf->lines[i + 1]);
                buf->bold_state[i] = buf->bold_state[i + 1];
            }
            buf->num_lines--;
            buf->lines[buf->num_lines][0] = '\0';
            buf->bold_state[buf->num_lines] = 0;
            return 1;
        }
    }
    return 0;
}

void buffer_find_replace(TextBuffer *buf, const char *same, const char *new_word) {
    if (!buf || !same || !new_word || strlen(same) == 0) return;

    int same_len = strlen(same);
    int new_len = strlen(new_word);
    int p = new_len - same_len;

    for (int current_line = 0; current_line < buf->num_lines; current_line++) {
        int s = 0;
        int line_length = strlen(buf->lines[current_line]);

        while (s < line_length) {
            // Skip whitespaces
            while (s < line_length && buf->lines[current_line][s] == ' ') {
                s++;
            }
            if (s >= line_length) {
                break;
            }

            int start = s;
            // Identify end of the word
            while (s < line_length && buf->lines[current_line][s] != ' ') {
                s++;
            }
            int word_length = s - start;

            // Extract potential matching word safely
            char temp[MAX_LINE_LENGTH];
            if (word_length >= MAX_LINE_LENGTH) {
                // Word is excessively long, skip to next character to avoid overflow
                s++;
                continue;
            }
            strncpy(temp, &buf->lines[current_line][start], word_length);
            temp[word_length] = '\0';

            if (strcmp(temp, same) == 0) {
                // Make sure we have buffer space to grow/shrink
                if (line_length + p >= MAX_LINE_LENGTH) {
                    // Line would exceed max length, skip replacement on this word
                    s = start + word_length;
                    continue;
                }

                if (p > 0) {
                    // Shift right to fit the longer new word
                    for (int i = line_length; i >= s; i--) {
                        buf->lines[current_line][i + p] = buf->lines[current_line][i];
                    }
                } else if (p < 0) {
                    // Shift left for the shorter new word
                    for (int i = start + same_len; i <= line_length; i++) {
                        buf->lines[current_line][i + p] = buf->lines[current_line][i];
                    }
                }

                // Copy replacement string and update lengths
                strncpy(&buf->lines[current_line][start], new_word, new_len);
                line_length += p;
                buf->lines[current_line][line_length] = '\0';
                s = start + new_len;
            } else {
                // Not a match, continue from current index
                s++;
            }
        }
    }
}

void buffer_toggle_bold(TextBuffer *buf, int line) {
    if (buf && line >= 0 && line < buf->num_lines) {
        buf->bold_state[line] = !buf->bold_state[line];
    }
}
