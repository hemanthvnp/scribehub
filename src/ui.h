#ifndef UI_H
#define UI_H

#include "buffer.h"

/* Displays the cool scrolling/fading list of key bindings */
void ui_display_bindings_intro(void);

/* Displays static key bindings inside ncurses screen */
void ui_display_bindings_help(void);

/* Performs renaming safely by updating the user buffer 'filename' */
void ui_rename_file(char *filename);

/* Renders a single line highlighting bold state if enabled */
void ui_display_line(int line_num, const TextBuffer *buf);

/* Starts the interactive editor session */
void ui_edit_file(TextBuffer *buf, char *filename);

#endif /* UI_H */
