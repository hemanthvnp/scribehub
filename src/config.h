#ifndef CONFIG_H
#define CONFIG_H

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 1000
#define MAX_FILENAME_LENGTH 100

/* Custom key bindings and ctrl-key mapping macros */
#ifndef CTRL
#define CTRL(c) ((c) & 0x1F)
#endif

#define KEY_SAVE_EXIT      CTRL('a')
#define KEY_SAVE           CTRL('p')
#define KEY_EXIT_NOSAVE    CTRL('o')
#define KEY_RENAME         CTRL('r')
#define KEY_TOGGLE_BOLD    27  /* Ctrl+[ or Escape */
#define KEY_AUTOSAVE       CTRL('t')
#define KEY_SHOW_BINDINGS  CTRL('w')
#define KEY_FIND_REPLACE   CTRL('y')
#define KEY_THEME_GREEN    CTRL('n')
#define KEY_THEME_YELLOW   CTRL('e')
#define KEY_THEME_NOTEPAD  CTRL('g')

#endif /* CONFIG_H */
