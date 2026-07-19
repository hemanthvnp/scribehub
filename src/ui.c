#include "ui.h"
#include <ncurses.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void ui_display_bindings_intro(void) {
    const char *text = "ScribeHub Key Bindings:\n"
                       "Ctrl+A - Save and exit\n"
                       "Ctrl+P - Save without exiting\n"
                       "Ctrl+O - Exit without saving\n"
                       "Ctrl+T - Autosave\n"
                       "Ctrl+R - Rename the current file\n"
                       "Ctrl+Y - Find and replace\n"
                       "Ctrl+[ - Bold the text\n"
                       "Ctrl+N - Switch to green theme\n"
                       "Ctrl+E - Switch to yellow theme\n"
                       "Ctrl+G - Switch to notepad theme\n"
                       "Arrow Keys - Navigate text\n"
                       "Enter - Insert newline\n"
                       "Backspace - Delete character before the cursor\n"
                       "Delete - Delete character at the cursor\n"
                       "Ctrl+W - Display the KeyBindings Again\n"
                       "Press any key to continue...";

    initscr();
    clear();
    const char *p = text;
    while (*p != '\0') {
        addch(*p);
        refresh();
        p++;
        napms(20);
    }
    getch();
    endwin();
}

void ui_display_bindings_help(void) {
    // Keep standard settings but temporarily hide the cursor
    curs_set(0);
    clear();
    mvprintw(0, 0, "ScribeHub Key Bindings:");
    mvprintw(2, 0, "Ctrl+A - Save and exit");
    mvprintw(3, 0, "Ctrl+P - Save without exiting");
    mvprintw(4, 0, "Ctrl+O - Exit without saving");
    mvprintw(5, 0, "Ctrl+T - Autosave");
    mvprintw(6, 0, "Ctrl+R - Rename the current file");
    mvprintw(7, 0, "Ctrl+Y - Find and replace");
    mvprintw(8, 0, "Ctrl+N - Switch to green theme");
    mvprintw(9, 0, "Ctrl+E - Switch to yellow theme");
    mvprintw(10, 0, "Ctrl+G - Switch to notepad theme");
    mvprintw(11, 0, "Arrow Keys - Navigate text");
    mvprintw(12, 0, "Enter - Insert newline");
    mvprintw(13, 0, "Backspace - Delete character before the cursor");
    mvprintw(14, 0, "Delete - Delete character at the cursor");
    mvprintw(15, 0, "Ctrl+[ - Bold the text");
    mvprintw(16, 0, "Ctrl+W - Display the KeyBindings Again");
    mvprintw(LINES - 2, 0, "Press any key to continue...");
    refresh();
    getch();
    curs_set(1);
}

void ui_rename_file(char *filename) {
    char new_filename[MAX_FILENAME_LENGTH];
    mvprintw(LINES - 2, 0, "Enter new filename: ");
    clrtoeol();
    echo();
    curs_set(1);
    getnstr(new_filename, MAX_FILENAME_LENGTH - 1);
    noecho();

    if (strlen(new_filename) > 0) {
        if (rename(filename, new_filename) == 0) {
            strcpy(filename, new_filename);
            mvprintw(LINES - 2, 0, "File renamed to %s", new_filename);
        } else {
            mvprintw(LINES - 2, 0, "Error renaming file. Press any key to continue.");
            getch();
        }
    } else {
        mvprintw(LINES - 2, 0, "Rename canceled.");
    }
    clrtoeol();
    refresh();
}

void ui_display_line(int line_num, const TextBuffer *buf) {
    if (buf->bold_state[line_num]) {
        attron(A_BOLD);
        mvprintw(line_num + 1, 0, "%d: %s", line_num + 1, buf->lines[line_num]);
        attroff(A_BOLD);
    } else {
        mvprintw(line_num + 1, 0, "%d: %s", line_num + 1, buf->lines[line_num]);
    }
}

void ui_edit_file(TextBuffer *buf, char *filename) {
    int current_line = 0;
    int current_col = 0;
    int current_theme = 1;
    int autosave_enabled = 0;
    time_t previous_autosave_time = time(NULL);

    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);  // Default Theme
    init_pair(2, COLOR_BLACK, COLOR_YELLOW); // Yellow Theme
    init_pair(3, COLOR_BLACK, COLOR_WHITE);  // Notepad Theme
    init_pair(4, COLOR_BLACK, COLOR_GREEN);  // Green Theme
    init_pair(5, COLOR_YELLOW, COLOR_BLACK); // Header Theme

    keypad(stdscr, TRUE);
    noecho();
    curs_set(1);

    while (1) {
        clear();

        // Apply theme
        if (current_theme == 1) {
            bkgd(COLOR_PAIR(1));
        } else if (current_theme == 2) {
            bkgd(COLOR_PAIR(4)); // Green theme
        } else if (current_theme == 3) {
            bkgd(COLOR_PAIR(2)); // Yellow theme
        } else if (current_theme == 4) {
            bkgd(COLOR_PAIR(3)); // Notepad theme
        }

        // Render header
        attron(COLOR_PAIR(5));
        mvprintw(0, 0, "ScribeHub - %s", filename);
        attroff(COLOR_PAIR(5));

        // Display body content
        for (int i = 0; i < buf->num_lines; i++) {
            ui_display_line(i, buf);
        }

        // Adjust cursor positioning
        move(current_line + 1, current_col + 3);

        // Autosave handling
        if (autosave_enabled && difftime(time(NULL), previous_autosave_time) >= 5) {
            buffer_save(buf, filename);
            mvprintw(LINES - 2, 0, "Autosaved to %s", filename);
            clrtoeol();
            refresh();
            previous_autosave_time = time(NULL);
        }

        int ch = getch();

        if (ch == KEY_SAVE_EXIT) {
            buffer_save(buf, filename);
            break;
        } else if (ch == KEY_SAVE) {
            buffer_save(buf, filename);
            mvprintw(LINES - 2, 0, "Saved to %s", filename);
            clrtoeol();
            refresh();
            napms(1000);
        } else if (ch == KEY_EXIT_NOSAVE) {
            break;
        } else if (ch == KEY_RENAME) {
            ui_rename_file(filename);
        } else if (ch == KEY_TOGGLE_BOLD) {
            buffer_toggle_bold(buf, current_line);
        } else if (ch == KEY_AUTOSAVE) {
            autosave_enabled = !autosave_enabled;
            if (autosave_enabled) {
                mvprintw(LINES - 2, 0, "Autosave enabled.");
            } else {
                mvprintw(LINES - 2, 0, "Autosave disabled.");
            }
            clrtoeol();
            refresh();
            sleep(1);
        } else if (ch == KEY_SHOW_BINDINGS) {
            ui_display_bindings_help();
        } else if (ch == KEY_FIND_REPLACE) {
            char same[MAX_LINE_LENGTH], new_word[MAX_LINE_LENGTH];
            mvprintw(LINES - 2, 0, "Enter word to replace: ");
            clrtoeol();
            echo();
            getnstr(same, MAX_LINE_LENGTH - 1);
            mvprintw(LINES - 3, 0, "Enter new word: ");
            clrtoeol();
            getnstr(new_word, MAX_LINE_LENGTH - 1);
            noecho();
            buffer_find_replace(buf, same, new_word);
        } else if (ch == KEY_THEME_GREEN) {
            current_theme = (current_theme == 2) ? 1 : 2;
        } else if (ch == KEY_THEME_YELLOW) {
            current_theme = (current_theme == 3) ? 1 : 3;
        } else if (ch == KEY_THEME_NOTEPAD) {
            current_theme = (current_theme == 4) ? 1 : 4;
        } else if (ch == KEY_UP) {
            if (current_line > 0) {
                current_line--;
                int prev_line_len = strlen(buf->lines[current_line]);
                if (current_col > prev_line_len) {
                    current_col = prev_line_len;
                }
            }
        } else if (ch == KEY_DOWN) {
            if (current_line < buf->num_lines - 1) {
                current_line++;
                int next_line_len = strlen(buf->lines[current_line]);
                if (current_col > next_line_len) {
                    current_col = next_line_len;
                }
            }
        } else if (ch == KEY_LEFT) {
            if (current_col > 0) {
                current_col--;
            }
        } else if (ch == KEY_RIGHT) {
            int line_len = strlen(buf->lines[current_line]);
            if (current_col < line_len) {
                current_col++;
            }
        } else if (ch == '\n') {
            buffer_insert_newline(buf, &current_line, &current_col);
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            buffer_delete_backspace(buf, &current_line, &current_col);
        } else if (ch == KEY_DC) {
            buffer_delete_char(buf, current_line, current_col);
        } else if (isprint(ch)) {
            if (buffer_insert_char(buf, current_line, current_col, ch)) {
                current_col++;
            }
        }
    }

    endwin();
}
