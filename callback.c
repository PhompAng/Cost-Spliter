#include <stdio.h>
#include "callback.h"

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;

    for (int i = 0; i < argc; i++) {
        printf("|%10.10s", argv[i] ? argv[i] : "NULL");
    }
    printf("|\n");
    return 0;
}

int callback_no_new_line(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;

    for (int i = 0; i < argc; i++) {
        printf("%10.10s", argv[i] ? argv[i] : "NULL");
    }
    printf("|");
    return 0;
}
