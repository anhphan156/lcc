#include "ast/ast_visualizer.h"
#include "ast/prepass.h"
#include "codegen/codegen.h"
#include "codegen/asm_file.h"
#include "lexer.h"
#include "parser/parser.h"
#include "symbol_table.h"
#include "sys/types.h"
#include "sys/wait.h"
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

    const char *asm_file_path = "code.s";
    asmfopen(asm_file_path);
    asm_preamble();

    open_dot_graph();

    lexical_scanner_setup(src, src_len);

    AstArray *aa = ast_parse();
    for (size_t i = 0; i < aa->len; i += 1) {
        struct ast_node *ast = aa->item[i];
        write_dot_graph(ast, i);
        prepass(ast);
        generate_code(ast);

        ast_clean(ast);
    }
    free(aa->item);

    close_dot_graph();
    asmfclose();
    clean_symbol_table();
    close_file(src, src_len);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        goto main_end;
    } else if (pid == 0) {
        int ret = execlp("gcc", "gcc", "-g", "-o", "out", asm_file_path, NULL);
        if (ret == -1) {
            perror("execvl");
            goto main_end;
        }
    }

    int status;
    waitpid(pid, &status, 0);

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
