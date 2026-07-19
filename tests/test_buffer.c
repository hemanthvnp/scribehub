#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test_buffer_creation_and_free(void) {
    printf("Running test_buffer_creation_and_free...\n");
    TextBuffer *buf = buffer_create();
    assert(buf != NULL);
    assert(buf->num_lines == 1);
    assert(strlen(buf->lines[0]) == 0);
    assert(buf->bold_state[0] == 0);
    buffer_free(buf);
    printf("Passed test_buffer_creation_and_free.\n\n");
}

void test_buffer_load_and_save(void) {
    printf("Running test_buffer_load_and_save...\n");
    const char *test_file = "tests/test_temp.txt";

    // Create a buffer and save some lines
    TextBuffer *buf = buffer_create();
    assert(buf != NULL);

    // Setup manual lines
    strcpy(buf->lines[0], "Line 1 content");
    buffer_insert_newline(buf, &(int){0}, &(int){14});
    strcpy(buf->lines[1], "Line 2 content");
    buf->num_lines = 2;

    int save_res = buffer_save(buf, test_file);
    assert(save_res == 1);
    buffer_free(buf);

    // Load content back
    TextBuffer *buf2 = buffer_create();
    int load_res = buffer_load(buf2, test_file);
    assert(load_res == 1);
    assert(buf2->num_lines == 2);
    assert(strcmp(buf2->lines[0], "Line 1 content") == 0);
    assert(strcmp(buf2->lines[1], "Line 2 content") == 0);
    buffer_free(buf2);

    // Clean up temporary file
    remove(test_file);
    printf("Passed test_buffer_load_and_save.\n\n");
}

void test_buffer_insert_char(void) {
    printf("Running test_buffer_insert_char...\n");
    TextBuffer *buf = buffer_create();
    assert(buf != NULL);

    // Insert 'a' at 0, 0
    int res = buffer_insert_char(buf, 0, 0, 'a');
    assert(res == 1);
    assert(strcmp(buf->lines[0], "a") == 0);

    // Insert 'b' at 0, 1
    res = buffer_insert_char(buf, 0, 1, 'b');
    assert(res == 1);
    assert(strcmp(buf->lines[0], "ab") == 0);

    // Insert 'c' at 0, 1 (middle insertion)
    res = buffer_insert_char(buf, 0, 1, 'c');
    assert(res == 1);
    assert(strcmp(buf->lines[0], "acb") == 0);

    // Invalidation check
    res = buffer_insert_char(buf, 1, 0, 'x'); // Invalid line index
    assert(res == 0);

    buffer_free(buf);
    printf("Passed test_buffer_insert_char.\n\n");
}

void test_buffer_insert_newline(void) {
    printf("Running test_buffer_insert_newline...\n");
    TextBuffer *buf = buffer_create();
    assert(buf != NULL);

    strcpy(buf->lines[0], "HelloWorld");
    int line = 0;
    int col = 5;

    // Split at "Hello" and "World"
    int res = buffer_insert_newline(buf, &line, &col);
    assert(res == 1);
    assert(buf->num_lines == 2);
    assert(strcmp(buf->lines[0], "Hello") == 0);
    assert(strcmp(buf->lines[1], "World") == 0);
    assert(line == 1);
    assert(col == 0);

    buffer_free(buf);
    printf("Passed test_buffer_insert_newline.\n\n");
}

void test_buffer_delete_backspace(void) {
    printf("Running test_buffer_delete_backspace...\n");
    TextBuffer *buf = buffer_create();
    assert(buf != NULL);

    strcpy(buf->lines[0], "Hello");
    int line = 0;
    int col = 5;

    // Delete 'o' from Hello
    int res = buffer_delete_backspace(buf, &line, &col);
    assert(res == 1);
    assert(strcmp(buf->lines[0], "Hell") == 0);
    assert(col == 4);

    // Let's add another line and test merge-up behavior
    buf->num_lines = 2;
    strcpy(buf->lines[1], "World");
    line = 1;
    col = 0;

    res = buffer_delete_backspace(buf, &line, &col);
    assert(res == 1);
    assert(buf->num_lines == 1);
    assert(strcmp(buf->lines[0], "HellWorld") == 0);
    assert(line == 0);
    assert(col == 4);

    buffer_free(buf);
    printf("Passed test_buffer_delete_backspace.\n\n");
}

void test_buffer_delete_char(void) {
    printf("Running test_buffer_delete_char...\n");
    TextBuffer *buf = buffer_create();
    assert(buf != NULL);

    strcpy(buf->lines[0], "Hello");

    // Delete character at index 1 ('e')
    int res = buffer_delete_char(buf, 0, 1);
    assert(res == 1);
    assert(strcmp(buf->lines[0], "Hllo") == 0);

    // Let's test merging with next line using Delete at the end of the line
    buf->num_lines = 2;
    strcpy(buf->lines[0], "Hllo");
    strcpy(buf->lines[1], "World");

    res = buffer_delete_char(buf, 0, 4); // Delete at end of "Hllo"
    assert(res == 1);
    assert(buf->num_lines == 1);
    assert(strcmp(buf->lines[0], "HlloWorld") == 0);

    buffer_free(buf);
    printf("Passed test_buffer_delete_char.\n\n");
}

void test_buffer_find_replace(void) {
    printf("Running test_buffer_find_replace...\n");
    TextBuffer *buf = buffer_create();
    assert(buf != NULL);

    strcpy(buf->lines[0], "the quick brown fox jumps over the lazy dog");
    buf->num_lines = 1;

    // Replace "fox" with "cat" (same size)
    buffer_find_replace(buf, "fox", "cat");
    assert(strcmp(buf->lines[0], "the quick brown cat jumps over the lazy dog") == 0);

    // Replace "cat" with "elephant" (larger size)
    buffer_find_replace(buf, "cat", "elephant");
    assert(strcmp(buf->lines[0], "the quick brown elephant jumps over the lazy dog") == 0);

    // Replace "elephant" with "ant" (smaller size)
    buffer_find_replace(buf, "elephant", "ant");
    assert(strcmp(buf->lines[0], "the quick brown ant jumps over the lazy dog") == 0);

    buffer_free(buf);
    printf("Passed test_buffer_find_replace.\n\n");
}

void test_buffer_toggle_bold(void) {
    printf("Running test_buffer_toggle_bold...\n");
    TextBuffer *buf = buffer_create();
    assert(buf != NULL);

    assert(buf->bold_state[0] == 0);
    buffer_toggle_bold(buf, 0);
    assert(buf->bold_state[0] == 1);
    buffer_toggle_bold(buf, 0);
    assert(buf->bold_state[0] == 0);

    buffer_free(buf);
    printf("Passed test_buffer_toggle_bold.\n\n");
}

int main(void) {
    printf("=================================\n");
    printf("Starting ScribeHub Unit Tests...\n");
    printf("=================================\n\n");

    test_buffer_creation_and_free();
    test_buffer_load_and_save();
    test_buffer_insert_char();
    test_buffer_insert_newline();
    test_buffer_delete_backspace();
    test_buffer_delete_char();
    test_buffer_find_replace();
    test_buffer_toggle_bold();

    printf("=================================\n");
    printf("All Tests Passed Successfully!\n");
    printf("=================================\n");
    return 0;
}
