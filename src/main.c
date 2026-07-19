#include "buffer.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // Allocate writable buffer for the filename to avoid modification of argv memory
    char filename[MAX_FILENAME_LENGTH];
    strncpy(filename, argv[1], MAX_FILENAME_LENGTH - 1);
    filename[MAX_FILENAME_LENGTH - 1] = '\0';

    // Show the intro fade-in keybindings list once on launch
    ui_display_bindings_intro();

    // Create the content buffer & load the file
    TextBuffer *buf = buffer_create();
    if (!buf) {
        fprintf(stderr, "Error: Failed to allocate memory for the text buffer.\n");
        return 1;
    }

    buffer_load(buf, filename);

    // Run the interactive editor session
    ui_edit_file(buf, filename);

    // Cleanup resources
    buffer_free(buf);

    return 0;
}
