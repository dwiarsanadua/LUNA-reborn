#include "bspatch.h"
#include <cstdio>
#include <cstring>

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: test_bspatch <oldfile> <newfile> <patchfile>\n");
        return 2;
    }
    if (bspatch_file(argv[1], argv[2], argv[3]) != 0) {
        fprintf(stderr, "bspatch failed\n");
        return 1;
    }
    return 0;
}
