#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include "show_data.h"
#include "callback.h"

sqlite3 *db;
sqlite3_stmt *stmt;

void show_owe() {
    char *err_msg = 0;
    int rc;
    char sql[999] = " ";
    rc = sqlite3_prepare_v2(db, "SELECT * FROM balance_detail", -1, &stmt, 0);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        exit(1);
    }

    int cols = sqlite3_column_count(stmt);
    char **name_ptr;
    int *balance_ptr;
    name_ptr = (char **)malloc(cols * sizeof(char *));
    balance_ptr = (int *)calloc(cols, sizeof(int *));

    for (int i=1; i<cols; i++) {
        name_ptr[i-1] = (char*)malloc(255);
        strcpy(name_ptr[i-1], sqlite3_column_name(stmt, i));
    }

    rc = sqlite3_prepare_v2(db, "SELECT * FROM balance_detail", -1, &stmt, 0);
    for (int i = 1; i < cols; i++) {
        *(balance_ptr+i-1) = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            *(balance_ptr+i-1) += sqlite3_column_int(stmt, i);
        }
    }

    //When I wrote this, only God and I understood what I was doing
    //Now, God only knows
    for (int i = 0; i < cols-1; ++i)
    {
        if (*(balance_ptr+i) == 0) {
            continue;
        }
        for (int j = i; j < cols-1; ++j)
        {
            if (*(balance_ptr+i) > 0 && *(balance_ptr+j) < 0) {
                if (abs(*(balance_ptr+i)) >= abs(*(balance_ptr+j))) {
                    printf("%s owe %s = %d\n", name_ptr[j], name_ptr[i], abs(*(balance_ptr+j)));
                    *(balance_ptr+i) += *(balance_ptr+j);
                    *(balance_ptr+j) = 0;
                } else {
                    printf("%s owe %s = %d\n", name_ptr[j], name_ptr[i], abs(*(balance_ptr+i)));
                    *(balance_ptr+j) += *(balance_ptr+i);
                    *(balance_ptr+i) = 0;
                }
            } else if (*(balance_ptr+i) < 0 && *(balance_ptr+j) > 0) {
                if (abs(*(balance_ptr+j)) >= abs(*(balance_ptr+i))) {
                    printf("%s owe %s = %d\n", name_ptr[i], name_ptr[j], abs(*(balance_ptr+i)));
                    *(balance_ptr+j) += *(balance_ptr+i);
                    *(balance_ptr+i) = 0;
                } else {
                    printf("%s owe %s = %d\n", name_ptr[i], name_ptr[j], abs(*(balance_ptr+j)));
                    *(balance_ptr+i) += *(balance_ptr+j);
                    *(balance_ptr+j) = 0;
                }
            }
        }
    }

    for (int i=0; i<cols-1; i++) {
        free(name_ptr[i]);
    }

    free(name_ptr);
    free(balance_ptr);

}

void show_balance() {
    char *err_msg = 0;
    int rc;
    char sql[999] = " ";
    rc = sqlite3_prepare_v2(db, "SELECT * FROM paid_detail", -1, &stmt, 0);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        exit(1);
    }

    int cols = sqlite3_column_count(stmt);

    printf(" ");
    for (int i=0; i<cols; i++) {
        if (i == cols-1) {
            printf("----------");
        } else {
            printf("-----------");
        }
    }
    printf("\n");
    printf("|%10.10s", "User");
    for (int i=1; i<cols; i++) {
        printf("|%10.10s", sqlite3_column_name(stmt, i));
    }
    printf("|\n|");
    for (int i=0; i<cols; i++) {
        if (i == cols-1) {
            printf("----------|");
        } else {
            printf("----------+");
        }
    }

    printf("\n|%10.10s|", "Paid");
    show_sum("paid_detail");

    printf("|%10.10s|", "Consumed");
    show_sum("spent_detail");

    printf("|%10.10s|", "Balance");
    show_sum("balance_detail");

    printf(" ");
    for (int i=0; i<cols; i++) {
        if (i == cols-1) {
            printf("----------");
        } else {
            printf("-----------");
        }
    }
    printf("\n");

    show_owe();
}

void show_sum(char table[]) {
    char *err_msg = 0;
    int rc;
    char sql[999] = " ";

    int cols = sqlite3_column_count(stmt);
    for (int i=1; i<cols; i++) {
        sprintf(sql, "SELECT sum(%s) FROM %s", sqlite3_column_name(stmt, i), table);
        rc = sqlite3_exec(db, sql, callback_no_new_line, 0, &err_msg);
        if (rc != SQLITE_OK ) {
            fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);

            sqlite3_free(err_msg);
            sqlite3_close(db);

            exit(1);
        }
    }
    printf("\n");
}

void show_table(char table[]) {
    char *err_msg = 0;
    int rc;
    char sql[999] = " ";

    sprintf(sql, "SELECT * FROM %s", table);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    int cols = sqlite3_column_count(stmt);

    printf(" ");
    for (int i=0; i<cols; i++) {
        if (i == cols-1) {
            printf("----------");
        } else {
            printf("-----------");
        }
    }
    printf("\n");
    for (int i=0; i<cols; i++) {
        printf("|%10.10s", sqlite3_column_name(stmt, i));
    }
    printf("|\n|");
    for (int i=0; i<cols; i++) {
        if (i == cols-1) {
            printf("----------|");
        } else {
            printf("----------+");
        }
    }
    printf("\n");

    sprintf(sql, "SELECT * FROM %s;", table);
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        exit(1);
    }
    printf(" ");
    for (int i=0; i<cols; i++) {
        if (i == cols-1) {
            printf("----------");
        } else {
            printf("-----------");
        }
    }
    printf("\n");
}
