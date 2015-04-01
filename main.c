#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void draw_main();
void add_expense(char[], double, char[], char[]);
void expense_manager();
void show_result();
void show_table(char[]);
void add_remove_user();
int callback(void *, int, char **, char **);
int callback1(void *, int, char **, char **);
sqlite3 *db;

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;

    for (int i = 0; i < argc; i++) {
        printf("%10.10s|", argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
int callback1(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;

    for (int i = 0; i < argc; i++) {
        printf("%10.10s|", argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

void add_expense(char name[], double amount, char datetime[], char payer_name[]) {
    int rc;
    char *err_msg = 0;
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
    char datetime[999];
    int number_of_splitter;
    char payer_name[999];
    char splitter_name[999];

    char *err_msg = 0;
    char sql[999] = " ";
    int rc;

    printf("%s\n", "1. Show Expense");
    printf("%s\n", "2. Add Expense");
    printf("%s\n", "3. Back");

    printf("%s", "Enter Choice: ");
    scanf("%d", &choice);

    system("clear");
    switch(choice){
        case 1:
            printf("=================================\n");
            printf("%10s|%10s|%10s|%10s|\n", "ID", "Name", "Amount", "Time");
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

            printf("=================================\n");
            printf("%10s|%10s|\n", "ID", "Username");
            show_table("user");

            printf("%s", "Payer name: ");
            scanf("%s", payer_name);

            printf("%s", "Number of Splitter: ");
            scanf("%d", &number_of_splitter);
            strcpy(sql, "INSERT INTO spent_detail(");
            for (int i = 1; i <= number_of_splitter; ++i)
            {
                printf("%s", "Splitter name: ");
                scanf("%s", splitter_name);
                strcat(sql, splitter_name);
                if (i == number_of_splitter) {
                    strcat(sql, ") VALUES (");
                } else {
                    strcat(sql, ",");
                }
            }
            for (int i = 1; i <= number_of_splitter; ++i)
            {
                strcat(sql, "'");
                split = amount/number_of_splitter;
                sprintf(split_str, "%.2lf", split);
                strcat(sql, split_str);
                if (i == number_of_splitter) {
                    strcat(sql, "');");
                } else {
                    strcat(sql, "',");
                }
            }

            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
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


void show_result() {
    sqlite3_stmt *stmt;
    int rc;
    char *err_msg = 0;
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
    for (int i=1; i<cols; i++) {
        sprintf(sql, "SELECT sum(%s) FROM paid_detail", sqlite3_column_name(stmt, i));
        rc = sqlite3_exec(db, sql, callback1, 0, &err_msg);
        if (rc != SQLITE_OK ) {
            fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);

            sqlite3_free(err_msg);
            sqlite3_close(db);

            exit(1);
        }
    }
    printf("\n");
    for (int i=1; i<cols; i++) {
        sprintf(sql, "SELECT sum(%s) FROM spent_detail", sqlite3_column_name(stmt, i));
        rc = sqlite3_exec(db, sql, callback1, 0, &err_msg);
        if (rc != SQLITE_OK ) {
            fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);

            sqlite3_free(err_msg);
            sqlite3_close(db);

            exit(1);
        }
    }
    printf("\n");

    printf("\n=================================\n");

}

void show_table(char table[]) {
    int rc;
    char *err_msg = 0;
    char sql[999] = " ";
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

void add_remove_user() {
    int choice;
    char name[999];
    char id[999];

    char *err_msg = 0;
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
            printf("=================================\n");
            printf("%10s|%10s|\n", "ID", "Username");
            show_table("user");
            add_remove_user();
            break;
        case 2:
            printf("%s", "Enter Username: ");
            scanf("%s", name);

            sprintf(sql, "INSERT INTO user(name) VALUES ('%s');", name);
            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
            system("clear");
            if (rc != SQLITE_OK ) {
                printf("SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
            } else {
                sprintf(sql, "ALTER TABLE spent_detail ADD COLUMN %s REAL DEFAULT 0;", name);
                rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
                sprintf(sql, "ALTER TABLE paid_detail ADD COLUMN %s REAL DEFAULT 0;", name);
                rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
                printf("User %s added successfully\n", name);
            }
            printf("=================================\n");
            add_remove_user();
            break;
        case 3:
            printf("=================================\n");
            printf("%10s|%10s|\n", "ID", "Username");
            show_table("user");
            printf("%s", "Enter ID: ");
            scanf("%s", id);

            sprintf(sql, "DELETE FROM user WHERE id=%s;", id);
            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
            system("clear");
            if (rc != SQLITE_OK ) {
                printf("SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
            } else {
                printf("ID %s deleted successfully\n", id);
            }
            printf("=================================\n");
            add_remove_user();
            break;
        case 4:
            draw_main();
            break;
        default:
            add_remove_user();
            break;
    }
}

void draw_main() {
    int choice;

    printf("%s\n", "1. Add/Remove User");
    printf("%s\n", "2. Add Expense");
    printf("%s\n", "3. Show Result");
    printf("%s\n", "0. exit");

    printf("%s", "Enter choice: ");
    scanf("%d", &choice);

    system("clear");
    switch(choice) {
        case 0:
            return;
            break;
        case 1:
            add_remove_user();
            break;
        case 2:
            expense_manager();
            break;
        case 3:
            show_result();
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
