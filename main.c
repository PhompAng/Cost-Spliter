#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void draw_main();
void add_expense(char[], double, char[], char[]);
void expense_manager();
void show_balance();
void show_table(char[]);
void add_user(char[]);
void remove_user(char[]);
void user_manager();
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
    char balance_str[999];
    char datetime[999];
    int number_of_splitter;
    char payer_name[999];
    char splitter_name[999];

    char *err_msg = 0;
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


void show_balance() {
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
    printf("%10.10s|", "User");
    for (int i=1; i<cols; i++) {
        printf("%10.10s|", sqlite3_column_name(stmt, i));
    }

    printf("\n%10.10s|", "Paid");
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

    printf("\n%10.10s|", "Consumed");
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

    printf("\n%10.10s|", "Balance");
    for (int i=1; i<cols; i++) {
        sprintf(sql, "SELECT sum(%s) FROM balance_detail", sqlite3_column_name(stmt, i));
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

void add_user(char name[]) {
    int rc;
    char *err_msg = 0;
    char sql[999] = " ";

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
        sprintf(sql, "ALTER TABLE balance_detail ADD COLUMN %s REAL DEFAULT 0;", name);
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
        printf("User %s added successfully\n", name);
    }
}

void remove_user(char id[]) {
    int rc;
    char *err_msg = 0;
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
            printf("=================================\n");
            printf("%10s|%10s|\n", "ID", "Username");
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
