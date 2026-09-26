#include <cplt/cplt.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define error(s) fprintf(stderr, (s));

#define KEY_ESCAPE(b, n) ((n) == 1 && (b)[0] == 0x1b)
#define KEY_ENTER(b) ((b)[0] == 0x0d)
#define KEY_BACKSPACE(b) ((b)[0] == 0x7f || (b)[0] == 0x08)

enum {
    NORMAL_MODE = 0,
    INSERT_MODE
};

typedef struct {
    char *keyword;
    color_t highlight;
} keyword_t;

char info[] = {
    "   ▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄▄  ▄▄▄▄▄▄▄▄▄▄▄▄\n"
    " ▄▓░▒░░░▓▓▒░▓  ▓▒▓▒░▒░▓████  ▓▒▓▒░▒░▓████\n"
    "▄▓░▒░░▓███▓▒█  ▀▀▀▒▒░▓█▀▀▀▀  ▀▀▀▒▒░▓█▀▀▀▀\n"
    "█░▒░█▀▀▀░██▓█     ░░▓██         ░░▓██    \n"
    "▓▒░░█   ▀▀▀▀▀     ▒▓███         ▒▓███    \n"
    "░░░▓█             ▓████         ▓████    \n"
    "▓░▓██   ▄▄▄▄▄     █████         █████    \n"
    "▒▓███▄▄▄▓█▓▓▓     ▓██▓▓         ▓██▓▓    \n"
    "░████████▓▓▒▒     ▒█▓▒▒         ▒█▓▒▒    \n"
    " ▀▓███▓▓▓▓▒░░     ░▓▒░░         ░▓▒░░    \n"
    "   ▀▀▀▀▀▀▀▀▀      ▀▀▀▀          ▀▀▀▀      V.1.0\n"
    "\n"
    "YOUR MINIMALISTIC, TERMINAL-BASED TEXTEDITOR\n"
    "\n"
};

keyword_t keyword_highlights[] = {
    {"int", RGB(255, 255, 0)},
    {"float", RGB(255, 255, 0)},
    {"double", RGB(255, 255, 0)},
    {"char", RGB(255, 255, 0)},
    {"short", RGB(255, 255, 0)},
    {"long", RGB(255, 255, 0)},
    {"bool", RGB(255, 255, 0)},
    {"void", RGB(255, 255, 0)},
    {"auto", RGB(255, 255, 0)},
    {"enum", RGB(255, 255, 0)},
    {"union", RGB(255, 255, 0)},
    {"struct", RGB(255, 255, 0)},
    {"register", RGB(255, 255, 0)},

    {"signed", RGB(255, 255, 0)},
    {"unsigned", RGB(255, 255, 0)},
    {"const", RGB(255, 255, 0)},
    {"static", RGB(255, 255, 0)},
    {"inline", RGB(255, 255, 0)},
    {"volatile", RGB(255, 255, 0)},
    {"typedef", RGB(255, 255, 0)},

    {"if",       RGB(0, 155, 255)},
    {"else",     RGB(0, 155, 255)},
    {"while",    RGB(0, 155, 255)},
    {"for",      RGB(0, 155, 255)},
    {"do",       RGB(0, 155, 255)},
    {"switch",   RGB(0, 155, 255)},
    {"case",     RGB(0, 155, 255)},
    {"break",    RGB(0, 155, 255)},
    {"continue", RGB(0, 155, 255)},
    {"return",   RGB(0, 155, 255)},
    {"default",  RGB(0, 155, 255)},
    {"extern",   RGB(255, 255, 0)},
    {"goto",     RGB(255, 255, 0)},

    {"sizeof", RGB(255, 255, 0)},
    {"NULL", RGB(255, 0, 0)},
    {"true", RGB(255, 0, 0)},
    {"false", RGB(255, 0, 0)},
    {"stdin", RGB(255, 0, 0)},
    {"stderr", RGB(255, 0, 0)},

    {"#define", RGB(125, 125, 125)},
    {"#include", RGB(125, 125, 125)},
    {"#ifdef", RGB(125, 125, 125)},
    {"#ifndef", RGB(125, 125, 125)},
    {"#else", RGB(125, 125, 125)},
    {"#endif", RGB(125, 125, 125)},
    {"#if", RGB(125, 125, 125)},
    {"#pragma once", RGB(125, 125, 125)},
};
color_t comment_color = RGB(125, 125, 125);
color_t string_lit_color = RGB(0, 255, 0);

char **buffer = NULL;
size_t buffer_cap = 0;
int cursor_x = 4;
int cursor_y = 0;
int row_offset = 0;
int edit_mode = NORMAL_MODE;

int *chars_written = NULL;
int rows_written = 1;

bool did_changes = false;

int rows, cols;

bool is_keyword_at(int x, int y, char *keyword) {
    int len = strlen(keyword);
    if (x + len > cols) {
        return false;
    }
    if (x > 0 && (isalpha(buffer[y][x - 1]) || isdigit(buffer[y][x - 1]))) {
        return false;
    }
    if (x + len + 1 <= cols && (isalpha(buffer[y][x + len]) || isdigit(buffer[y][x + len]))) {
        return false;
    }
    if (strncmp(&buffer[y][x], keyword, len) != 0) {
        return false;
    }
    return true;
}

void ensure_buffer_capacity(size_t needed_cap) {
    if (needed_cap > buffer_cap) {
        size_t prev_cap = buffer_cap;
        buffer_cap = needed_cap * 2;
        buffer = realloc(buffer, buffer_cap * sizeof(char *));
        chars_written = realloc(chars_written, buffer_cap * sizeof(int));
        for (size_t i = prev_cap; i < buffer_cap; ++i) {
            buffer[i] = malloc(cols);
            memset(buffer[i], ' ', cols);
            chars_written[i] = 0;
        }
    }
}

int main(int argc, char **argv) {
    if (argc > 2) {
        error("Usage: ctt <new_filename>\n");
        return 1;
    } else if (argc < 2) {
        printf("%s", info);
        return 0;
    }
    if (!cplt_begin()) {
        error("Failed setting terminal to raw mode\n");
        return 1;
    }

    rows = cplt_get_screen_width();
    cols = cplt_get_screen_height();
    
    buffer_cap = rows > 100 ? rows : 100;
    buffer = malloc(buffer_cap * sizeof(char *));
    chars_written = calloc(buffer_cap, sizeof(int));
    for (size_t i = 0; i < buffer_cap; ++i) {
        buffer[i] = malloc(cols);
        memset(buffer[i], ' ', cols);
    }

    FILE *file_exist = fopen(argv[1], "r");
    if (file_exist) {
        char line[1024] = {0};
        int i = 0;
        while (fgets(line, sizeof(line), file_exist)) {
            ensure_buffer_capacity(i + 1);
            int len_line = strlen(line);
            if (len_line > 0 && line[len_line - 1] == '\n') line[len_line - 1] = '\0';
            int n = (strlen(line) > cols - 4 ? cols - 4 : strlen(line));
            memcpy(&buffer[i][4], line, n);
            chars_written[i] = n;
            i++;
        }
        rows_written = i > 0 ? i : 1;
        fclose(file_exist);
    }

    FILE *file = fopen(argv[1], "wb");
    if (!file) {
        perror("");
        return 1;
    }

    int screen_rows = rows - 1;

    while (1) {
        unsigned char key_buffer[3] = {0};
        ssize_t n = 0;
        cplt_get_key_pressed(key_buffer, &n);

        if (KEY_ESCAPE(key_buffer, n)) {
            if (edit_mode == INSERT_MODE) {
                edit_mode = NORMAL_MODE;
            }
        } else if (n == 1) {
            if (KEY_ENTER(key_buffer) && edit_mode == INSERT_MODE) {
                ensure_buffer_capacity(rows_written + 1);
                for (int y = rows_written; y > cursor_y; y--) {
                    memcpy(buffer[y], buffer[y - 1], cols);
                    chars_written[y] = chars_written[y - 1];
                }
                cursor_y++;
                cursor_x = 4;
                memset(buffer[cursor_y], ' ', cols);
                chars_written[cursor_y] = 0;
                rows_written++;
                did_changes = true;
                if (cursor_y >= row_offset + screen_rows) {
                    row_offset = cursor_y - screen_rows + 1;
                }
            } else if (KEY_BACKSPACE(key_buffer) && cursor_x >= 4 && edit_mode == INSERT_MODE) {
                if (cursor_y >= rows_written - 1 && chars_written[cursor_y] <= 0 && cursor_y > 0) {
                    rows_written--;
                    cursor_y--;
                    cursor_x = chars_written[cursor_y] + 4;
                    did_changes = true;
                    if (cursor_y < row_offset) {
                        row_offset = cursor_y;
                    }
                } else if (cursor_y > 0 && chars_written[cursor_y] <= 0) {
                    for (int i = cursor_y + 1; i < rows_written; ++i) {
                        memcpy(&buffer[i - 1][4], &buffer[i][4], cols - 4);
                        chars_written[i - 1] = chars_written[i];
                    }
                    rows_written--;
                    cursor_y--;
                    cursor_x = chars_written[cursor_y] + 4;
                    did_changes = true;
                    if (cursor_y < row_offset) {
                        row_offset = cursor_y;
                    }
                } else if (cursor_x > 4) {
                    memmove(&buffer[cursor_y][cursor_x - 1], &buffer[cursor_y][cursor_x], chars_written[cursor_y] + 4 - cursor_x);
                    buffer[cursor_y][chars_written[cursor_y] + 4 - 1] = ' ';
                    cursor_x--;
                    chars_written[cursor_y]--;
                    did_changes = true;
                }
            } else {
                if (edit_mode == INSERT_MODE) {
                    if (key_buffer[0] == '\t') {
                        for (int i = 0; i < 4; ++i) {
                            if (chars_written[cursor_y] < cols - 4 - 1) {
                                if (cursor_x >= chars_written[cursor_y] + 4) {
                                    buffer[cursor_y][cursor_x++] = ' ';
                                } else {
                                    int bytes_to_move = (chars_written[cursor_y] + 4) - cursor_x;
                                    memmove(&buffer[cursor_y][cursor_x + 1], &buffer[cursor_y][cursor_x], bytes_to_move);
                                    buffer[cursor_y][cursor_x++] = ' ';
                                }
                                chars_written[cursor_y]++;
                            }
                        }
                        did_changes = true;
                    } else {
                        if (chars_written[cursor_y] < cols - 4 - 1) {
                            if (cursor_x >= chars_written[cursor_y] + 4) {
                                buffer[cursor_y][cursor_x++] = key_buffer[0];
                            } else {
                                int bytes_to_move = (chars_written[cursor_y] + 4) - cursor_x;
                                memmove(&buffer[cursor_y][cursor_x + 1], &buffer[cursor_y][cursor_x], bytes_to_move);
                                buffer[cursor_y][cursor_x++] = key_buffer[0];
                            }
                            chars_written[cursor_y]++;
                            did_changes = true;
                        }
                    }
                } else if (edit_mode == NORMAL_MODE) {
                    switch(key_buffer[0]) {
                    case 'i':
                        edit_mode = INSERT_MODE;
                        break;
                    case 'q': {
                        FILE *dummy = freopen(argv[1], "wb", file);
                        for (int i = 0; i < rows_written; ++i) {
                            fwrite(&buffer[i][4], 1, chars_written[i], file);
                            char newline = '\n';
                            fwrite(&newline, 1, 1, file);
                        }
                        fclose(file);
                        cplt_end();
                        for (size_t i = 0; i < buffer_cap; ++i) {
                            free(buffer[i]);
                        }
                        free(buffer);
                        free(chars_written);
                        exit(0);
                        break;
                    }
                    case 'w': {
                        FILE *dummy = freopen(argv[1], "wb", file);
                        for (int i = 0; i < rows_written; ++i) {
                            fwrite(&buffer[i][4], 1, chars_written[i], file);
                            char newline = '\n';
                            fwrite(&newline, 1, 1, file);
                        }
                        fflush(file);
                        did_changes = false;
                        break;
                    }
                    case '0':
                        cursor_x = 4;
                        break;
                    case '$':
                        cursor_x = chars_written[cursor_y] + 4;
                        break;
                    case 't':
                        cursor_x = 4;
                        cursor_y = 0;
                        if (cursor_y < row_offset) {
                            row_offset = cursor_y;
                        }
                        break;
                    case 'n':
                        cursor_x = 4;
                        cursor_y += screen_rows;
                        cursor_y = cursor_y >= rows_written ? rows_written - 1 : cursor_y;
                        row_offset += screen_rows;
                        {
                            int max_offset = rows_written - screen_rows;
                            if (max_offset < 0) max_offset = 0;
                            if (row_offset > max_offset) {
                                row_offset = max_offset;
                            }
                        }
                        break;
                    case 'p':
                        cursor_x = 4;
                        cursor_y -= screen_rows;
                        cursor_y = cursor_y < 0 ? 0 : cursor_y;
                        row_offset -= screen_rows;
                        if (row_offset < 0) {
                            row_offset = 0;
                        }
                        break;
                    case 'b':
                        cursor_x = 4;
                        cursor_y = rows_written - 1;
                        if (cursor_y >= row_offset + screen_rows) {
                            row_offset = cursor_y - screen_rows + 1;
                        }
                        break;
                    case 'h':
                        if (cursor_x > 4) {
                            cursor_x--;
                        }
                        break;
                    case 'l':
                        if (cursor_x < chars_written[cursor_y] + 4 && cursor_x < cols - 1) {
                            cursor_x++;
                        }
                        break;
                    case 'k':
                        if (cursor_y > 0) {
                            bool end_of_line = cursor_x >= chars_written[cursor_y] + 4;
                            cursor_y--;
                            if (cursor_y < row_offset) {
                                row_offset = cursor_y;
                            }
                            if (end_of_line) {
                                cursor_x = chars_written[cursor_y] + 4;
                            } else {
                                cursor_x = cursor_x > chars_written[cursor_y] + 4 ? chars_written[cursor_y] + 4 : cursor_x;
                            }
                        }
                        break;
                    case 'j':
                        if (cursor_y < rows_written - 1) {
                            bool end_of_line = cursor_x >= chars_written[cursor_y] + 4;
                            cursor_y++;
                            if (cursor_y >= row_offset + screen_rows) {
                                row_offset = cursor_y - screen_rows + 1;
                            }
                            if (end_of_line) {
                                cursor_x = chars_written[cursor_y] + 4;
                            } else {
                                cursor_x = cursor_x > chars_written[cursor_y] + 4 ? chars_written[cursor_y] + 4 : cursor_x;
                            }
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        for (int screen_y = 0; screen_y < screen_rows; ++screen_y) {
            int file_y = row_offset + screen_y;
            char line_buffer[4] = {0};
            if (file_y < rows_written) {
                snprintf(line_buffer, sizeof(line_buffer), "%-2d ", file_y + 1);
            } else {
                snprintf(line_buffer, sizeof(line_buffer), "~");
            }
            cplt_puts(0, screen_y, line_buffer, RGB(55, 55, 55), RGB(0, 0, 0));
        }
        
        cplt_puts(0, rows - 1, edit_mode == NORMAL_MODE ? " NORMAL " : " INSERT ", RGB(0, 0, 0), edit_mode == NORMAL_MODE ? RGB(255, 255, 255) : RGB(0, 255, 150));
        cplt_puts(8, rows - 1, argv[1], RGB(0, 0, 0), RGB(0, 200, 255));
        if (did_changes) {
            cplt_puts(8 + strlen(argv[1]), rows - 1, " (Edited)", RGB(0, 0, 0), RGB(0, 200, 255));
        } else {
            cplt_puts(8 + strlen(argv[1]), rows - 1, "         ", RGB(255, 255, 255), RGB(0, 0, 0));
        }

        bool multiline_comment = false;
        for (int screen_y = 0; screen_y < screen_rows; ++screen_y) {
            int file_y = row_offset + screen_y;
            int keep_color = 0;
            color_t highlight_color = multiline_comment ? comment_color : RGB(255, 255, 255);
            bool string_lit = false;
            bool comment = false;

            for (int x = 4; x < cols; ++x) {
                color_t text_color = RGB(255, 255, 255);
                color_t bg_color = RGB(0, 0, 0);

                if (file_y < rows_written) {
                    if (x < cols - 1 && buffer[file_y][x] == '/' && buffer[file_y][x + 1] == '/') {
                        highlight_color = comment_color;
                        comment = true;
                    } else if (x < cols - 1 && buffer[file_y][x] == '/' && buffer[file_y][x + 1] == '*') {
                        highlight_color = comment_color;
                        multiline_comment = true;
                    } else if (x < cols - 1 && buffer[file_y][x] == '*' && buffer[file_y][x + 1] == '/') {
                        highlight_color = comment_color;
                        keep_color = 2;
                        multiline_comment = false;
                    } else if (buffer[file_y][x] == '\"' && !comment && !multiline_comment) {
                        highlight_color = string_lit_color;
                        if (string_lit) {
                            keep_color = 1;
                        }
                        string_lit = !string_lit;
                    } else if (buffer[file_y][x] == '\'' && !comment && !multiline_comment) {
                        highlight_color = string_lit_color;
                        if (string_lit) {
                            keep_color = 1;
                        }
                        string_lit = !string_lit;
                    } else if (buffer[file_y][x] == '<' && is_keyword_at(4, file_y, "#include") && !comment && !multiline_comment) {
                        highlight_color = string_lit_color;
                        string_lit = true;
                    } else if (buffer[file_y][x] == '>' && is_keyword_at(4, file_y, "#include") && !comment && !multiline_comment) {
                        highlight_color = string_lit_color;
                        keep_color = 1;
                        string_lit = false;
                    } else if (!comment && !multiline_comment && !string_lit) {
                        for (size_t i = 0; i < sizeof(keyword_highlights) / sizeof(*keyword_highlights); ++i) {
                            if (is_keyword_at(x, file_y, keyword_highlights[i].keyword)) {
                                highlight_color = keyword_highlights[i].highlight;
                                keep_color = strlen(keyword_highlights[i].keyword);
                            }
                        }
                    }
                }
                if (x == cursor_x && file_y == cursor_y) {
                    text_color = RGB(0, 0, 0);
                    bg_color = RGB(255, 255, 255);
                    if (keep_color > 0) {
                        keep_color--;
                    }
                } else if (keep_color > 0) {
                    keep_color--;
                    text_color = highlight_color;
                } else if (string_lit || comment || multiline_comment) {
                    text_color = highlight_color;
                }

                char ch = (file_y < rows_written) ? buffer[file_y][x] : ' ';
                cplt_putc(x, screen_y, ch, text_color, bg_color);
            }
        }
        cplt_refresh();
    }
    fclose(file);
    cplt_end();
    for (size_t i = 0; i < buffer_cap; ++i) {
        free(buffer[i]);
    }
    free(buffer);
    free(chars_written);
}
