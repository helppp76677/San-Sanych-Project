#include <stdio.h>
#include <string.h>
#include "analysis.h"

int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("%s --help\n",argv[0]);
        printf("%s schema <csv-file>\n",argv[0]);
        printf("%s stats <csv-file>\n",argv[0]);
        return 0;
    }
    else if (argc == 3 && strcmp(argv[1], "schema") == 0) {
        return run_schema(argv[2]);
    }
    else if (argc == 3 && strcmp(argv[1], "stats") == 0) {
        return run_stats(argv[2]);
    }
    else{
        fprintf(stderr,"Неверные аргументы. Используйте %s --help\n",argv[0]);
        return 2;
    }
}
