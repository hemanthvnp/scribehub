#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#define MAX_LINES 1000 // Maximum number of lines
#define MAX_LINE_LENGTH 1000 // Maximum line length
#define MAX_FILENAME_LENGTH 100 // Maximum filename length

void saveFile(char* filename, char *content[], int num_lines) 
{
    FILE *file = fopen(filename, "w");
    if (file != NULL) 
    {
        for (int i = 0; i < num_lines; i++) 
        {
            fputs(content[i], file);  // Write the line
            fputc('\n', file);        // Add newline after each line
        }
        fclose(file);
    }
    refresh();
}

void displayKeyBindings() 
{
    char text[] = "ScribeHub Key Bindings:\n"
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
                  "Ctrl + W - Display the KeyBindings Again\n"
                  "Press any key to continue...";

    char *p = text;
    initscr();
    clear();
    while (*p != '\0')
    {
        addch(*p);
        refresh();
        p++;
        napms(20);
    }
    getch();
    endwin();
}

void displayKeyBindings1() 
{
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    keypad(stdscr, TRUE);
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
    mvprintw(15, 0, "Ctrl+[ - Bold the text\n");
    mvprintw(16, 0, "Ctrl + W - Display the KeyBindings Again");
    mvprintw(LINES - 2, 0, "Press any key to continue...");
    refresh();
    getch();
    endwin();
    curs_set(1);
}

void renameFile(char* old_filename)
{
    char new_filename[MAX_FILENAME_LENGTH];
    mvprintw(LINES - 2, 0, "Enter new filename: ");
    echo();  // Enable echoing for user input
    curs_set(1);  // Show the cursor
    getnstr(new_filename, MAX_FILENAME_LENGTH - 1); // Get the new filename from the user
    noecho(); //Disable echo
    if (rename(old_filename, new_filename) == 0)
    {
        strcpy(old_filename, new_filename);
        mvprintw(LINES - 2, 0, "File renamed to %s", new_filename);
    }
    else
    {
        mvprintw(LINES - 2, 0, "Error renaming file. Press any key to continue.");
        getch();
    }
    refresh();
}

void displayLine(int line_num, char *content[], int bold_state[]) {
    if (bold_state[line_num]) {
        // Display line number and content in bold
        attron(A_BOLD); // Turn on bold attribute
        mvprintw(line_num + 1, 0, "%d: %s", line_num + 1, content[line_num]); // Line number + content
        attroff(A_BOLD); // Turn off bold attribute
    } else {
        // Display line number and content normally
        mvprintw(line_num + 1, 0, "%d: %s", line_num + 1, content[line_num]); // Line number + content
    }
}

void findreplace(char *content[], const char *same, const char *new, int num_lines) 
{
    for (int current_line = 0; current_line < num_lines; current_line++) 
    {
        int s = 0;
        int line_length = strlen(content[current_line]);
        while (s < line_length) 
        {
            while (s < line_length && content[current_line][s] == ' ') 
            {
                s++;
            }
            if (s >= line_length)
            {
                break;
            }
            int start = s;
            while (s < line_length && content[current_line][s] != ' ') 
            {
                s++;
            }
            int word_length = s - start;
            char temp[MAX_LINE_LENGTH];
            strncpy(temp, &content[current_line][start], word_length);
            temp[word_length] = '\0';
            if (strcmp(temp, same) == 0) 
            {
                int sameLen = strlen(same);
                int newLen = strlen(new);
                int p = newLen - sameLen;
                if (p > 0) 
                { 
                    for (int i = line_length; i >= s; i--) 
                    {
                        content[current_line][i + p] = content[current_line][i];
                    }
                } 
                else if (p < 0) 
                { 
                    for (int i = start + sameLen; i <= line_length; i++) 
                    {
                        content[current_line][i + p] = content[current_line][i];
                    }
                }
                strncpy(&content[current_line][start], new, newLen);
                line_length += p;
                content[current_line][line_length] = '\0';
                s = start + newLen;
            } 
            else 
            {
                s++;
            }
        }
    }
}
void editFile(char* filename) 
{
    char *content[MAX_LINE_LENGTH];
    int num_lines = 0;

    // Allocate memory for each line
    for (int i = 0; i < MAX_LINES; i++) 
    {
        content[i] = malloc(MAX_LINE_LENGTH);
        if (content[i] == NULL) {
            perror("Memory allocation failed");
            exit(1);
        }
    }

    // Open the file and read its content
    FILE *file = fopen(filename, "r");
    if (file != NULL) 
    {
        while (fgets(content[num_lines], MAX_LINE_LENGTH, file) && num_lines < MAX_LINES) 
        {
            content[num_lines][strcspn(content[num_lines], "\n")] = '\0';  // Remove newline character
            num_lines++;
        }
        fclose(file);
    }
    if (num_lines == 0) 
    {
        strcpy(content[0], ""); // First line
        num_lines = 1; // Update number of lines
    }

    int current_line = 0;
    int current_col = 0;
    int current_theme = 1; // Start with default theme
    int autosave_enabled = 0;
    time_t previous_autosave_time = time(NULL);
    int bold_state[MAX_LINES] = {0};

    // Start ncurses mode
    initscr();
    start_color();  // Initialize color functionality
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // Default theme
    init_pair(2, COLOR_BLACK, COLOR_YELLOW); //Yellow theme
    init_pair(3, COLOR_BLACK, COLOR_WHITE); // Notepad Theme
    init_pair(4, COLOR_BLACK, COLOR_GREEN); // Green Theme
    init_pair(5, COLOR_YELLOW, COLOR_BLACK); //Highlight the application name and file name

    keypad(stdscr, TRUE);  // Enable special keys
    noecho();               // Disable echoing of typed characters
    curs_set(1);           // Show the cursor

    while (1) 
    {
        clear();  // Clear the screen
        
        // Set the background based on the current theme
        if (current_theme == 1) 
        {
            bkgd(COLOR_PAIR(1)); // Apply default theme
        } 
        else if (current_theme == 2) 
        {
            bkgd(COLOR_PAIR(4)); // Apply green theme
        }
        else if (current_theme == 3)
        {
            bkgd(COLOR_PAIR(2)); // Apply yellow theme
        }
        else if (current_theme == 4)
        {
            bkgd(COLOR_PAIR(3)); //Apply Notepad theme
        }

        // Highlight ScribeHub and filename at the top of the screen
        attron(COLOR_PAIR(5));  // Turn on color pair
        mvprintw(0, 0, "ScribeHub - %s", filename);
        attroff(COLOR_PAIR(5)); // Turn off color pair

        // Display the content
        for (int i = 0; i < num_lines; i++) 
        {
            displayLine(i, content, bold_state); // Use displayLine
        }

        // Move the cursor to the current position
        move(current_line + 1, current_col + 3);

        // Autosave Function
        if (autosave_enabled && difftime(time(NULL), previous_autosave_time) >= 5) 
        {
            saveFile(filename, content, num_lines);
            mvprintw(LINES - 2, 0, "Autosaved to %s", filename);
            refresh();
            previous_autosave_time = time(NULL);
        }

        int ch = getch();  // Get user input

        if (ch == 1)  // Ctrl+A to save and exit
        {  
            saveFile(filename, content, num_lines);
            break;  // Exit the loop after saving
        }
        else if (ch == 16)  // Ctrl+P to save without quitting
        {  
            saveFile(filename, content, num_lines);
        }
        else if (ch == 15)  // Ctrl+O to quit without saving
        {  
            break;  // Exit without saving
        }
        else if (ch == 18)  // Ctrl+R to rename file
        {  
            renameFile(filename);  // Rename the file
        }
        else if (ch == 27)  // Ctrl+[ to toggle bold
        {
            bold_state[current_line] = !bold_state[current_line]; // Toggle bold state
        }
        else if (ch == 20) // Ctrl+T to autosave the file
        {
            autosave_enabled = !autosave_enabled;
            if (autosave_enabled)
            {
                mvprintw(LINES - 2, 0, "Autosave enabled.");
            }
            else
            {
                mvprintw(LINES - 2, 0, "Autosave disabled.");
            }
            refresh();
            sleep(2);
        }
        else if (ch==23) //Ctrl+W to display Key Bindings
        {
            displayKeyBindings1();
        }
        else if (ch == 25) //Ctrl+Y to find and replace
        {
            char same[MAX_LINE_LENGTH], new[MAX_LINE_LENGTH];
            mvprintw(LINES - 2, 0, "Enter word to replace: ");
            echo();
            getnstr(same, MAX_LINE_LENGTH - 1);
            mvprintw(LINES - 3, 0, "Enter new word: ");
            getnstr(new, MAX_LINE_LENGTH - 1);
            noecho();
            findreplace(content, same, new, num_lines);
        }
        else if (ch == 14)  // Ctrl+N to change the theme to green
        {
            if(current_theme == 1)
            {
                current_theme = 2; // Set the theme
            }
            else
            {
                current_theme = 1; // Reset the theme
            }
        }
        else if (ch == 5) //Ctrl+E to change the theme to yellow
        {
            if(current_theme == 1)
            {
                current_theme = 3; // Set the theme
            }
            else
            {
                current_theme = 1; // Reset the theme
            }
        }
        else if (ch == 7) //Ctrl+G to change the theme to Notepad theme
        {
            if(current_theme == 1)
            {
                current_theme = 4; // Set the theme
            }
            else
            {
                current_theme = 1; // Reset the theme
            }
        }
        else if (ch == KEY_UP)  // Move cursor up
        {  
            if (current_line > 0) // If current line is the starting line then move up is not possible
            {
                current_line--;
                if (current_col > strlen(content[current_line])) 
                {
                    current_col = strlen(content[current_line]); //Moves to the end of the upper line
                }
            }
        }
        else if (ch == KEY_DOWN)  // Move cursor down
        {  
            if (current_line < num_lines - 1) // If current line is ending line then it cannot move down further
            {
                current_line++;
                if (current_col > strlen(content[current_line])) 
                {
                    current_col = strlen(content[current_line]); //Moves to the end of the lower line
                }
            }
        }
        else if (ch == KEY_LEFT)  // Move cursor left
        {  
            if (current_col > 0) 
            {
                current_col--;
            }
        }
        else if (ch == KEY_RIGHT)  // Move cursor right
        {  
            if (current_col < strlen(content[current_line])) 
            {
                current_col++;
            }
        }
        else if (ch == '\n')  // Enter key (newline)
        {  
            if (num_lines < MAX_LINES) 
            {
                for (int i = num_lines; i > current_line; i--) 
                {
                    strcpy(content[i], content[i - 1]);  // Shift lines down
                }
                content[current_line + 1][0] = '\0';  // Initialize the new line
                num_lines++;
                current_line++;
                current_col = 0;  // Reset column to the start of the new line
            }
        }
        else if (ch == KEY_BACKSPACE)  // Backspace
        {  
            if (current_col > 0) 
            {
                // Cursor is in the middle of a line
                current_col--;
                for (int i = current_col; i < strlen(content[current_line]); i++) 
                {
                    content[current_line][i] = content[current_line][i + 1];  // Shift characters left
                }
            }
            else if (current_line > 0 && current_col == 0) 
            {
                // Cursor is at the beginning of a line and there are previous lines
                int prev_len = strlen(content[current_line - 1]);  // Length of the previous line
                // Check if the combined length of the two lines is within the limit
                if (prev_len + strlen(content[current_line]) < MAX_LINE_LENGTH) 
                {
                    strcat(content[current_line - 1], content[current_line]);
                    for (int i = current_line; i < num_lines - 1; i++) 
                    {
                        strcpy(content[i], content[i + 1]);
                    }
                    num_lines--;
                    current_line--;
                    current_col = prev_len;
                }
            }
        }
        else if (ch == KEY_DC)  // Delete
        {
            int line_length = strlen(content[current_line]);
            if (current_col < line_length) //Character at the middle
            {
                for (int i = current_col; i < line_length; i++) 
                {
                    content[current_line][i] = content[current_line][i + 1];
                }
            } 
            else if (current_line < num_lines - 1) //Character at the end
            {
                strcat(content[current_line], content[current_line + 1]);
                for (int i = current_line + 1; i < num_lines - 1; i++) 
                {
                    strcpy(content[i], content[i + 1]);
                }
                num_lines--;
                content[num_lines][0] = '\0';
            }
        }
        else if (isprint(ch))  // Insert character
        {

            if (current_col < MAX_LINE_LENGTH - 1) 
            {
                for (int i = strlen(content[current_line]); i >= current_col; i--) 
                {
                    content[current_line][i + 1] = content[current_line][i];
                }
                content[current_line][current_col] = ch;
                current_col++;
            }
        }
    }

    // Clear the memory
    for (int i = 0; i < MAX_LINES; i++) 
    {
        free(content[i]);
    }
    endwin();  // End ncurses mode
}

int main(int argc, char* argv[]) // if arguement less than 2, the format of the arguement will be printed otherwise edit the file
{
    if (argc < 2) 
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    displayKeyBindings();
    editFile(argv[1]);  // Start editing the file

    return 0;
}
