#include "ast/ast_visualizer.h"
#include "codegen.h"
#include "lexer.h"
#include "parser/parser.h"
#include "symbol_table.h"
#include "sys/types.h"
#include "token.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static char *read_file(const char *, size_t *);
static void  close_file(char *, size_t);

int main(int argc, char **argv) {
    char *src_file_path = argv[optind++];

    size_t src_len = 0;
    char  *src     = read_file(src_file_path, &src_len);

    lexical_scanner_setup(src, src_len);

    // struct token token;
    // while (not_end()) {
    //     if (lexical_scan(&token) != -1) {
    //         printf("Token: %.*s on line %d\n", (int)token.lexeme_length, token.lexeme_start, token.line);
    //     }
    // }

    struct ast_node *expr = ast_parse();
    // ast_print(expr, 1);
    write_dot_graph(expr);
    // generate_code(expr);
    ast_clean(expr);
    clean_symbol_table();
    close_file(src, src_len);

    // pid_t pid = fork();
    // if (pid == -1) {
    //     perror("fork");
    //     goto main_end;
    // } else if (pid == 0) {
    //     int ret = execlp("gcc", "gcc", "-o", "out", "code.s", NULL);
    //     if (ret == -1) {
    //         perror("execvl");
    //         goto main_end;
    //     }
    // }

main_end:
    return 0;
}

static char *read_file(const char *path, size_t *buffer_length) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    struct stat st;

    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        exit(1);
    }

    if (st.st_size == 0) {
        fprintf(stderr, "File %s is empty\n", path);
    }

    char *buffer = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }

    if (close(fd) == -1) {
        perror("close");
    }

    *buffer_length = st.st_size;
    return buffer;
}

static void close_file(char *buffer, size_t len) {
    if (munmap(buffer, len) == -1) {
        perror("munmap");
    }
}
