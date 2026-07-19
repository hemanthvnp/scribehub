#ifndef BUFFER_H
#define BUFFER_H

#include "config.h"

typedef struct {
    char *lines[MAX_LINES];
    int num_lines;
    int bold_state[MAX_LINES];
} TextBuffer;

/* Initialize a text buffer with empty or default content */
TextBuffer* buffer_create(void);

/* Free all resources allocated for the text buffer */
void buffer_free(TextBuffer *buf);

/* Load file content into the text buffer */
int buffer_load(TextBuffer *buf, const char *filename);

/* Save buffer content to a file */
int buffer_save(const TextBuffer *buf, const char *filename);

/* Insert a character at the specified line and column */
int buffer_insert_char(TextBuffer *buf, int line, int col, char ch);

/* Insert a newline at the specified line and column (splits the line) */
int buffer_insert_newline(TextBuffer *buf, int *line, int *col);

/* Delete the character before the cursor (backspace) */
int buffer_delete_backspace(TextBuffer *buf, int *line, int *col);

/* Delete the character at the cursor (delete key) */
int buffer_delete_char(TextBuffer *buf, int line, int col);

/* Search and replace occurrences of 'same' with 'new_word' in the buffer */
void buffer_find_replace(TextBuffer *buf, const char *same, const char *new_word);

/* Toggle bold state of a line */
void buffer_toggle_bold(TextBuffer *buf, int line);

#endif /* BUFFER_H */
