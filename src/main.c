#include <cplt/cplt.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define error(s) fprintf(stderr, (s));

#define KEY_ESCAPE(b, n) ((n) == 1 && (b)[0] == 0x1b)
#define KEY_ENTER(b) ((b)[0] == 0x0d)
#define KEY_BACKSPACE(b) ((b)[0] == 0x7f || (b)[0] == 0x08)

#define X_OFF 7

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
    "   ▀▀▀▀▀▀▀▀▀      ▀▀▀▀          ▀▀▀▀      V.1.1\n"
    "\n"
    "YOUR MINIMALISTIC, TERMINAL-BASED TEXTEDITOR\n"
    "\n"
};

keyword_t keyword_highlights[] = {
    // Macros
    {"#define",         RGB(125, 125, 125)},
    {"#include",        RGB(125, 125, 125)},
    {"#ifdef",          RGB(125, 125, 125)},
    {"#ifndef",         RGB(125, 125, 125)},
    {"#else",           RGB(125, 125, 125)},
    {"#endif",          RGB(125, 125, 125)},
    {"#if",             RGB(125, 125, 125)},
    {"#pragma once",    RGB(125, 125, 125)},

    // Data types
    {"int",             RGB(0, 225, 255)},
    {"float",           RGB(0, 225, 255)},
    {"double",          RGB(0, 225, 255)},
    {"char",            RGB(0, 225, 255)},
    {"short",           RGB(0, 225, 255)},
    {"long",            RGB(0, 225, 255)},
    {"bool",            RGB(0, 225, 255)},
    {"void",            RGB(0, 225, 255)},
    {"auto",            RGB(0, 225, 255)},
    {"enum",            RGB(0, 225, 255)},
    {"union",           RGB(0, 225, 255)},
    {"struct",          RGB(0, 225, 255)},
    {"register",        RGB(0, 225, 255)},

    {"signed",          RGB(0, 225, 255)},
    {"unsigned",        RGB(0, 225, 255)},
    {"const",           RGB(0, 225, 255)},
    {"static",          RGB(0, 225, 255)},
    {"inline",          RGB(0, 225, 255)},
    {"volatile",        RGB(0, 225, 255)},
    {"typedef",         RGB(0, 225, 255)},

    {"if",              RGB(0, 155, 255)},
    {"else",            RGB(0, 155, 255)},
    {"while",           RGB(0, 155, 255)},
    {"for",             RGB(0, 155, 255)},
    {"do",              RGB(0, 155, 255)},
    {"switch",          RGB(0, 155, 255)},
    {"case",            RGB(0, 155, 255)},
    {"break",           RGB(0, 155, 255)},
    {"continue",        RGB(0, 155, 255)},
    {"return",          RGB(0, 155, 255)},
    {"default",         RGB(0, 155, 255)},
    {"extern",          RGB(0, 155, 255)},
    {"goto",            RGB(0, 155, 255)},

    // libc
    {"sizeof",          RGB(0, 155, 255)},
    {"NULL",            RGB(255, 55, 0)},
    {"true",            RGB(255, 55, 0)},
    {"false",           RGB(255, 55, 0)},
    {"stdin",           RGB(255, 55, 0)},
    {"stderr",          RGB(255, 55, 0)},
    {"size_t",          RGB(0, 225, 255)},
    {"int8_t",          RGB(0, 225, 255)},
    {"int16_t",         RGB(0, 255, 255)},
    {"int32_t",         RGB(0, 255, 255)},
    {"int64_t",         RGB(0, 255, 255)},
    {"uint8_t",         RGB(0, 255, 255)},
    {"uint16_t",        RGB(0, 255, 255)},
    {"uint32_t",        RGB(0, 255, 255)},
    {"uint64_t",        RGB(0, 255, 255)},
    {"FILE",            RGB(0, 255, 255)},
    {"printf",          RGB(155, 0, 255)},
    {"fprintf",         RGB(155, 0, 255)},
    {"memmove",         RGB(155, 0, 255)},
    {"memcpy",          RGB(155, 0, 255)},
    {"memset",          RGB(155, 0, 255)},
    {"strlen",          RGB(155, 0, 255)},
    {"strcmp",          RGB(155, 0, 255)},
    {"strncmp",         RGB(155, 0, 255)},
    {"strcat",          RGB(155, 0, 255)},
    {"strncat",         RGB(155, 0, 255)},
    {"fopen",           RGB(155, 0, 255)},
    {"freopen",         RGB(155, 0, 255)},
    {"fclose",          RGB(155, 0, 255)},
    {"fwrite",          RGB(155, 0, 255)},
    {"fread",           RGB(155, 0, 255)},
    {"fflush",          RGB(155, 0, 255)},
    {"sprintf",         RGB(155, 0, 255)},
    {"snprintf",        RGB(155, 0, 255)},
    {"fgets",           RGB(155, 0, 255)},
    {"isalpha",         RGB(155, 0, 255)},
    {"isdigit",         RGB(155, 0, 255)},
    {"malloc",          RGB(155, 0, 255)},
    {"calloc",          RGB(155, 0, 255)},
    {"free",            RGB(155, 0, 255)},
    {"realloc",         RGB(155, 0, 255)},
};
color_t comment_color = RGB(125, 125, 125);
color_t macro_color = RGB(125, 125, 125);
color_t string_lit_color = RGB(0, 255, 155);

char **buffer = NULL;
size_t buffer_cap = 0;
int cursor_x = X_OFF;
int cursor_y = 0;
int row_offset = 0;
int edit_mode = NORMAL_MODE;
int line_number_jump = -1; // if not -1 and enter is hit, go to this line number

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
            if (len_line > 0 && line[len_line - 1] == '\n') {
                line[len_line - 1] = '\0';
            }
            int n = (strlen(line) > cols - X_OFF ? cols - X_OFF : strlen(line));
            memcpy(&buffer[i][X_OFF], line, n);
            chars_written[i] = n;
            i++;
        }
        rows_written = i > 0 ? i : 1;
        fclose(file_exist);
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
            if (KEY_ENTER(key_buffer)) {
                if (edit_mode == INSERT_MODE) {
                    ensure_buffer_capacity(rows_written + 1);
                    for (int y = rows_written; y > cursor_y; y--) {
                        memcpy(buffer[y], buffer[y - 1], cols);
                        chars_written[y] = chars_written[y - 1];
                    }
                    cursor_y++;
                    cursor_x = X_OFF;
                    memset(buffer[cursor_y], ' ', cols);
                    chars_written[cursor_y] = 0;
                    rows_written++;
                    did_changes = true;
                    if (cursor_y >= row_offset + screen_rows) {
                        row_offset = cursor_y - screen_rows + 1;
                    }
                } else if (edit_mode == NORMAL_MODE && line_number_jump > 0) {
                    cursor_y = line_number_jump - 1;
                    cursor_x = X_OFF;
                    if (cursor_y >= rows_written) {
                        cursor_y = rows_written - 1;
                    }
                    if (cursor_y < row_offset) {
                        row_offset = cursor_y;
                    }
                    if (cursor_y >= row_offset + screen_rows) {
                        row_offset = cursor_y - screen_rows + 1;
                    }
                    line_number_jump = -1;
                }
            } else if (KEY_BACKSPACE(key_buffer) && cursor_x >= X_OFF && edit_mode == INSERT_MODE) {
                if (cursor_y >= rows_written - 1 && chars_written[cursor_y] <= 0 && cursor_y > 0) {
                    rows_written--;
                    cursor_y--;
                    cursor_x = chars_written[cursor_y] + X_OFF;
                    did_changes = true;
                    if (cursor_y < row_offset) {
                        row_offset = cursor_y;
                    }
                } else if (cursor_y > 0 && chars_written[cursor_y] <= 0) {
                    for (int i = cursor_y + 1; i < rows_written; ++i) {
                        memcpy(&buffer[i - 1][X_OFF], &buffer[i][X_OFF], cols - X_OFF);
                        chars_written[i - 1] = chars_written[i];
                    }
                    rows_written--;
                    cursor_y--;
                    cursor_x = chars_written[cursor_y] + X_OFF;
                    did_changes = true;
                    if (cursor_y < row_offset) {
                        row_offset = cursor_y;
                    }
                } else if (cursor_x > X_OFF) {
                    memmove(&buffer[cursor_y][cursor_x - 1], &buffer[cursor_y][cursor_x], chars_written[cursor_y] + X_OFF - cursor_x);
                    buffer[cursor_y][chars_written[cursor_y] + X_OFF - 1] = ' ';
                    cursor_x--;
                    chars_written[cursor_y]--;
                    did_changes = true;
                }
            } else {
                if (edit_mode == INSERT_MODE) {
                    if (key_buffer[0] == '\t') {
                        int steps = 4 - ((cursor_x - X_OFF) % 4);
                        for (int i = 0; i < steps; ++i) {
                            if (chars_written[cursor_y] < cols - X_OFF - 1) {
                                if (cursor_x >= chars_written[cursor_y] + X_OFF) {
                                    buffer[cursor_y][cursor_x++] = ' ';
                                } else {
                                    int bytes_to_move = (chars_written[cursor_y] + X_OFF) - cursor_x;
                                    memmove(&buffer[cursor_y][cursor_x + 1], &buffer[cursor_y][cursor_x], bytes_to_move);
                                    buffer[cursor_y][cursor_x++] = ' ';
                                }
                                chars_written[cursor_y]++;
                            }
                        }
                        did_changes = true;
                    } else {
                        if (chars_written[cursor_y] < cols - X_OFF - 1) {
                            if (cursor_x >= chars_written[cursor_y] + X_OFF) {
                                buffer[cursor_y][cursor_x++] = key_buffer[0];
                            } else {
                                int bytes_to_move = (chars_written[cursor_y] + X_OFF) - cursor_x;
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
                        line_number_jump = -1;
                        edit_mode = INSERT_MODE;
                        break;
                    case 'q': {
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
                        FILE *file = fopen(argv[1], "wb");
                        if (!file) {
                            perror("Failed writing to file");
                            break;
                        }
                        for (int i = 0; i < rows_written; ++i) {
                            fwrite(&buffer[i][X_OFF], 1, chars_written[i], file);
                            char newline = '\n';
                            fwrite(&newline, 1, 1, file);
                        }
                        fflush(file);
                        fclose(file);
                        file = NULL;
                        did_changes = false;
                        break;
                    }
                    case 'D':
                        if (cursor_y > 0) {
                            for (int i = cursor_y + 1; i < rows_written; ++i) {
                                memcpy(&buffer[i - 1][X_OFF], &buffer[i][X_OFF], cols - X_OFF);
                                chars_written[i - 1] = chars_written[i];
                            }
                            rows_written--;
                            cursor_y--;
                            cursor_x = X_OFF;
                            did_changes = true;
                            if (cursor_y < row_offset) {
                                row_offset = cursor_y;
                            }
                        } else if (cursor_y == 0 && chars_written[0] > 0) {
                            cursor_x = X_OFF;
                            memset(&buffer[0][X_OFF], ' ', cols - X_OFF);
                            chars_written[0] = 0;
                        }
                        break;
                    case 'd':
                        if (cursor_y > 0) {
                            for (int i = cursor_y + 1; i < rows_written; ++i) {
                                memcpy(&buffer[i - 1][X_OFF], &buffer[i][X_OFF], cols - X_OFF);
                                chars_written[i - 1] = chars_written[i];
                            }
                            rows_written--;
                            if (cursor_y >= rows_written) {
                                cursor_y = rows_written - 1;
                            }   
                            cursor_x = X_OFF;
                            did_changes = true;
                            if (cursor_y < row_offset) {
                                row_offset = cursor_y;
                            }
                        } else if (cursor_y == 0 && chars_written[0] > 0) {
                            cursor_x = X_OFF;
                            memset(&buffer[0][X_OFF], ' ', cols - X_OFF);
                            chars_written[0] = 0;
                        }
                        break;
                    case 's':
                        cursor_x = X_OFF;
                        break;
                    case 'e':
                        cursor_x = chars_written[cursor_y] + X_OFF;
                        break;
                    case 't':
                        cursor_x = X_OFF;
                        cursor_y = 0;
                        if (cursor_y < row_offset) {
                            row_offset = cursor_y;
                        }
                        break;
                    case 'n':
                        cursor_x = X_OFF;
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
                        cursor_x = X_OFF;
                        cursor_y -= screen_rows;
                        cursor_y = cursor_y < 0 ? 0 : cursor_y;
                        row_offset -= screen_rows;
                        if (row_offset < 0) {
                            row_offset = 0;
                        }
                        break;
                    case 'c':                
                        ensure_buffer_capacity(rows_written + 1);
                        for (int y = rows_written; y > cursor_y; y--) {
                            memcpy(buffer[y], buffer[y - 1], cols);
                            chars_written[y] = chars_written[y - 1];
                        }
                        cursor_y++;
                        cursor_x = X_OFF;
                        memcpy(&buffer[cursor_y][X_OFF], &buffer[cursor_y - 1][X_OFF], cols - X_OFF);
                        chars_written[cursor_y] = chars_written[cursor_y - 1];
                        rows_written++;
                        did_changes = true;
                        if (cursor_y >= row_offset + screen_rows) {
                            row_offset = cursor_y - screen_rows + 1;
                        }
                        break;
                    case 'b':
                        cursor_x = X_OFF;
                        cursor_y = rows_written - 1;
                        if (cursor_y >= row_offset + screen_rows) {
                            row_offset = cursor_y - screen_rows + 1;
                        }
                        break;
                    case 'h':
                        if (cursor_x > X_OFF) {
                            cursor_x--;
                        }
                        break;
                    case 'l':
                        if (cursor_x < chars_written[cursor_y] + X_OFF && cursor_x < cols - 1) {
                            cursor_x++;
                        }
                        break;
                    case 'k':
                        if (cursor_y > 0) {
                            if (line_number_jump <= 0) {
                                bool end_of_line = cursor_x >= chars_written[cursor_y] + X_OFF;
                                cursor_y--;
                                if (cursor_y < row_offset) {
                                    row_offset = cursor_y;
                                }
                                if (end_of_line) {
                                    cursor_x = chars_written[cursor_y] + X_OFF;
                                } else {
                                    cursor_x = cursor_x > chars_written[cursor_y] + X_OFF ? chars_written[cursor_y] + X_OFF : cursor_x;
                                }
                            } else {
                                cursor_x = X_OFF;
                                if (cursor_y + 1 > line_number_jump) {
                                    cursor_y -= line_number_jump;
                                } else {
                                    cursor_y = 0;
                                }
                                if (cursor_y < row_offset) {
                                    row_offset = cursor_y;
                                }
                                line_number_jump = -1;
                            }
                        }
                        break;
                    case 'j':
                        if (cursor_y < rows_written - 1) {
                            if (line_number_jump <= 0) {
                                bool end_of_line = cursor_x >= chars_written[cursor_y] + X_OFF;
                                cursor_y++;
                                if (cursor_y >= row_offset + screen_rows) {
                                    row_offset = cursor_y - screen_rows + 1;
                                }
                                if (end_of_line) {
                                    cursor_x = chars_written[cursor_y] + X_OFF;
                                } else {
                                    cursor_x = cursor_x > chars_written[cursor_y] + X_OFF ? chars_written[cursor_y] + X_OFF : cursor_x;
                                }
                            } else {
                                cursor_x = X_OFF;
                                if (rows_written - cursor_y > line_number_jump) {
                                    cursor_y += line_number_jump;
                                } else {
                                    cursor_y = rows_written - 1;
                                }
                                if (cursor_y >= row_offset + screen_rows) {
                                    row_offset = cursor_y - screen_rows + 1;
                                }
                                line_number_jump = -1;
                            }
                        }
                        break;
                    default:
                        if (isdigit(key_buffer[0])) {
                            if (line_number_jump < 0) {
                                line_number_jump = key_buffer[0] - '0';
                            } else {
                                line_number_jump *= 10;
                                line_number_jump += (key_buffer[0] - '0');
                            }

                        }
                        break;
                    }
                }
            }
        }

        for (int screen_y = 0; screen_y < screen_rows; ++screen_y) {
            int file_y = row_offset + screen_y;
            char line_buffer[7] = {0};
            if (file_y < rows_written) {
                snprintf(line_buffer, sizeof(line_buffer), "%5d ", file_y + 1);
            } else {
                snprintf(line_buffer, sizeof(line_buffer), "    ~ ");
            }
            cplt_puts(0, screen_y, line_buffer, RGB(55, 55, 55), RGB(0, 0, 0));
        }
        
        cplt_puts(0, rows - 1, edit_mode == NORMAL_MODE ? " NORMAL " : " INSERT ", RGB(0, 0, 0), edit_mode == NORMAL_MODE ? RGB(255, 255, 255) : RGB(0, 255, 150));
        for (int x = 8; x < cols; ++x) {
            cplt_putc(x, rows - 1, ' ', RGB(255, 255, 255), RGB(45, 45, 45));
        }
        cplt_puts(8 + 1, rows - 1, argv[1], RGB(255, 255, 255), RGB(45, 45, 45));
        {
            char pos[256] = {0};
            snprintf(pos, sizeof(pos), " %5d:%-3d (%d%%)", cursor_y + 1, cursor_x - X_OFF + 1, (int)((float)(cursor_y + 1) / rows_written * 100.0f));
            cplt_puts(8 + 1 + strlen(argv[1]), rows - 1, pos, RGB(255, 255, 255), RGB(45, 45, 45));

            if (did_changes) {
                cplt_puts(8 + 1 + strlen(argv[1]) + strlen(pos) + 1, rows - 1, "(Edited)", RGB(255, 255, 255), RGB(45, 45, 45));
            }
        }

        {
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            char date[256] = {0};
            snprintf(date, sizeof(date), "%02d/%02d/%04d %02d:%02d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min);
            cplt_puts(cols - 1 - strlen(date), rows - 1, date, RGB(255, 255, 255), RGB(45, 45, 45));
        }

        bool multiline_comment = false;
        for (int screen_y = 0; screen_y < screen_rows; ++screen_y) {
            int file_y = row_offset + screen_y;
            int keep_color = 0;
            color_t highlight_color = multiline_comment ? comment_color : RGB(255, 255, 255);
            bool string_lit = false;
            bool comment = false;

            for (int x = X_OFF; x < cols; ++x) {
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
                    } else if (buffer[file_y][x] == '<' && is_keyword_at(X_OFF, file_y, "#include") && !comment && !multiline_comment) {
                        highlight_color = string_lit_color;
                        string_lit = true;
                    } else if (buffer[file_y][x] == '>' && is_keyword_at(X_OFF, file_y, "#include") && !comment && !multiline_comment) {
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
    cplt_end();
    for (size_t i = 0; i < buffer_cap; ++i) {
        free(buffer[i]);
    }
    free(buffer);
    free(chars_written);
    return 0;
}
