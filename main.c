#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void draw_main();
void add_expense(char[], double, char[], char[]);
void expense_manager();
void show_owe();
void show_balance();
void show_sum(char[]);
void show_table(char[]);
void alter_tabel(char[], char[]);
void add_user(char[]);
void remove_user(char[]);
void user_manager();
int callback(void *, int, char **, char **);
int callback_no_new_line(void *, int, char **, char **);
sqlite3 *db;
sqlite3_stmt *stmt;
char *err_msg = 0;

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;

    for (int i = 0; i < argc; i++) {
        printf("%10.10s|", argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
int callback_no_new_line(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;

    for (int i = 0; i < argc; i++) {
        printf("%10.10s|", argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

void add_expense(char name[], double amount, char datetime[], char payer_name[]) {
    int rc;
    char sql[999] = " ";
    sprintf(sql, "INSERT INTO expense(name, amount, datetime) VALUES ('%s', '%.2lf', '%s');", name, amount, datetime);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    system("clear");
    if (rc != SQLITE_OK ) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        sprintf(sql, "INSERT INTO paid_detail(%s) VALUES ('%.2lf');", payer_name, amount);
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
        if (rc != SQLITE_OK ) {
            printf("SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);
        } else {
            printf("Add Expense successfully\n");
            printf("name: %s\namount: %.2lf\npayer: %s\n", name, amount, payer_name);
        }
    }
    printf("=================================\n");
}

void expense_manager() {
    int choice;

    char name[999];
    double amount;
    double split;
    char split_str[999];
    char balance_str[999];
    char datetime[999];
    int number_of_splitter;
    char payer_name[999];
    char splitter_name[999];

    char sql[999] = " ";
    char sql1[999] = " ";
    int rc;

    printf("%s\n", "1. Show Expense");
    printf("%s\n", "2. Add Expense");
    printf("%s\n", "3. Back");

    printf("%s", "Enter Choice: ");
    scanf("%d", &choice);

    system("clear");
    switch(choice){
        case 1:
            show_table("expense");
            expense_manager();
            break;
        case 2:
            printf("%s", "Expense Name: ");
            scanf(" %[^\n]s", name);

            printf("%s", "Amount: ");
            scanf("%lf", &amount);

            printf("%s", "Time: ");
            scanf(" %[^\n]s", datetime);

            show_table("user");

            printf("%s", "Payer name: ");
            scanf("%s", payer_name);
            sprintf(sql, "INSERT INTO balance_detail(%s) VALUES ('%.2lf')", payer_name, amount);
            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

            printf("%s", "Number of Splitter: ");
            scanf("%d", &number_of_splitter);
            strcpy(sql, "INSERT INTO spent_detail(");
            strcpy(sql1, "INSERT INTO balance_detail(");
            for (int i = 1; i <= number_of_splitter; ++i)
            {
                printf("%s", "Splitter name: ");
                scanf("%s", splitter_name);
                strcat(sql, splitter_name);
                strcat(sql1, splitter_name);
                if (i == number_of_splitter) {
                    strcat(sql, ") VALUES (");
                    strcat(sql1, ") VALUES (");
                } else {
                    strcat(sql, ",");
                    strcat(sql1, ",");
                }
            }
            for (int i = 1; i <= number_of_splitter; ++i)
            {
                strcat(sql, "'");
                strcat(sql1, "'");
                split = amount/number_of_splitter;
                sprintf(split_str, "%.2lf", split);
                sprintf(balance_str, "%.2lf", -split);
                strcat(sql, split_str);
                strcat(sql1, balance_str);
                if (i == number_of_splitter) {
                    strcat(sql, "');");
                    strcat(sql1, "');");
                } else {
                    strcat(sql, "',");
                    strcat(sql1, "',");
                }
            }

            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
            rc = sqlite3_exec(db, sql1, 0, 0, &err_msg);
            if (rc != SQLITE_OK ) {
                printf("SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
            } else {
                printf("Add Expense successfully\n");
            }

            add_expense(name, amount, datetime, payer_name);
            expense_manager();
            break;
        case 3:
            draw_main();
            break;
        default:
            expense_manager();
            break;
    }
}

void show_owe() {
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
    printf("%10.10s|", "User");
    for (int i=1; i<cols; i++) {
        printf("%10.10s|", sqlite3_column_name(stmt, i));
    }

    printf("\n%10.10s|", "Paid");
    show_sum("paid_detail");

    printf("%10.10s|", "Consumed");
    show_sum("spent_detail");

    printf("%10.10s|", "Balance");
    show_sum("balance_detail");

    show_owe();

    printf("\n=================================\n");

}

void show_sum(char table[]) {
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
    int rc;
    char sql[999] = " ";

    sprintf(sql, "SELECT * FROM %s", table);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    int cols = sqlite3_column_count(stmt);

    for (int i=0; i<cols; i++) {
        printf("%10.10s|", sqlite3_column_name(stmt, i));
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
    printf("=================================\n");
}

void alter_tabel(char table[], char name[]) {
    int rc;
    char sql[999] = " ";
    sprintf(sql, "ALTER TABLE %s ADD COLUMN %s REAL DEFAULT 0;", table, name);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

void add_user(char name[]) {
    int rc;
    char sql[999] = " ";

    sprintf(sql, "INSERT INTO user(name) VALUES ('%s');", name);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    system("clear");
    if (rc != SQLITE_OK ) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        alter_tabel("spent_detail", name);
        alter_tabel("paid_detail", name);
        alter_tabel("balance_detail", name);
        printf("User %s added successfully\n", name);
    }
}

void remove_user(char id[]) {
    int rc;
    char sql[999] = " ";


    // remove_user() doesn't remove column in paid_detail, spent_detail, and balance_detail
    sprintf(sql, "DELETE FROM user WHERE id=%s;", id);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    system("clear");
    if (rc != SQLITE_OK ) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("ID %s deleted successfully\n", id);
    }
}

void user_manager() {
    int choice;
    char name[999];
    char id[999];

    char sql[999] = " ";
    int rc;

    printf("%s\n", "1. Show User");
    printf("%s\n", "2. Add User");
    printf("%s\n", "3. Remove User");
    printf("%s\n", "4. Back");

    printf("%s", "Enter choice: ");
    scanf("%d", &choice);

    system("clear");
    switch(choice) {
        case 1:
            show_table("user");

            user_manager();
            break;
        case 2:
            printf("%s", "Enter Username: ");
            scanf("%s", name);

            add_user(name);
            printf("=================================\n");

            user_manager();
            break;
        case 3:
            show_table("user");

            printf("%s", "Enter ID: ");
            scanf("%s", id);

            remove_user(id);
            printf("=================================\n");

            user_manager();
            break;
        case 4:
            draw_main();
            break;
        default:
            user_manager();
            break;
    }
}

void draw_main() {
    int choice;

    printf("%s\n", "1. User Manager");
    printf("%s\n", "2. Expense Manager");
    printf("%s\n", "3. Show Balance");
    printf("%s\n", "0. Exit");

    printf("%s", "Enter choice: ");
    scanf("%d", &choice);

    system("clear");
    switch(choice) {
        case 0:
            return;
            break;
        case 1:
            user_manager();
            break;
        case 2:
            expense_manager();
            break;
        case 3:
            show_balance();
            draw_main();
            break;
        default:
            draw_main();
            break;
    }
}

int main(void) {
    int rc = sqlite3_open("ljdata.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n",
        sqlite3_errmsg(db));
        sqlite3_close(db);

        exit(0);
    }

    system("clear");
    draw_main();
    sqlite3_close(db);
    printf("%s\n", "BYE");
    return 0;
}
