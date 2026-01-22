#include "lexer.h"
#include "token.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
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

    struct token token;

    while (not_end()) {
        if (lexical_scan(&token) != -1) {
            switch (token.type) {
            case T_DOUBLELIT:
                printf("Token: %f on line %d\n", token.value.doubleval, token.line);
                break;
            case T_INTLIT:
                printf("Token: %ld on line %d\n", token.value.intval, token.line);
                break;
            default:
                printf("Token: enum(%d) on line %d\n", token.type, token.line);
                break;
            }
        }
    }

    close_file(src, src_len);

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
