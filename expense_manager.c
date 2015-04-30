#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "expense_manager.h"
#include "draw_main.h"
#include "show_data.h"

sqlite3 *db;
sqlite3_stmt *stmt;
char *err_msg = 0;

void add_expense() {
    int rc;

    char name[999];
    double amount;
    double split;
    char split_str[999];
    char balance_str[999];

    time_t timer;
    struct tm* tm_info;
    char datetime[26];

    time(&timer);
    tm_info = localtime(&timer);
    strftime(datetime, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    int number_of_splitter;
    char payer_name[999];
    char splitter_name[999];

    char sql[999] = " ";
    char sql1[999] = " ";

    printf("%s", "Expense Name: ");
    scanf(" %[^\n]s", name);

    printf("%s", "Amount: ");
    scanf("%lf", &amount);

    show_table("user");

    printf("%s", "Payer name: ");
    scanf("%s", payer_name);
    sprintf(sql, "INSERT INTO balance_detail(%s) VALUES ('%.2lf')", payer_name, amount);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        system("clear");
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        expense_manager();
    }

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
        system("clear");
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        expense_manager();
    }

    sprintf(sql, "INSERT INTO expense(name, amount, payer_name, datetime) VALUES ('%s', '%.2lf', '%s', '%s');", name, amount, payer_name, datetime);
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
            printf("%s\n", "Add Expense successfully");
            printf("%s\n", "------------------------");
            printf("name: %s\namount: %.2lf\npayer: %s\n", name, amount, payer_name);
        }
    }
}

void remove_expense(char id[]) {
    char *err_msg = 0;
    int rc;
    char sql[999] = " ";
    int num;

    sprintf(sql, "DELETE FROM expense WHERE id=%s;", id);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    system("clear");
    if (rc != SQLITE_OK ) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    sprintf(sql, "DELETE FROM paid_detail WHERE id=%s;", id);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    sprintf(sql, "DELETE FROM spent_detail WHERE id=%s;", id);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    num = atoi(id);
    num *= 2;
    sprintf(id, "%d", num);
    sprintf(sql, "DELETE FROM balance_detail WHERE id=%s;", id);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    num = atoi(id);
    num -= 1;
    sprintf(id, "%d", num);
    sprintf(sql, "DELETE FROM balance_detail WHERE id=%s;", id);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("ID %s deleted successfully\n", id);
    }
}

void expense_manager() {
    int choice;
    char id[999];

    printf("%s\n", "═══╦════════════════════════════════════════════════════════════════════════════");
    printf("%s\n", " 1 │ Show Expense");
    printf("%s\n", " 2 │ Add Expense");
    printf("%s\n", " 3 | Remove Expense");
    printf("%s\n", "───┼────────────────────────────────────────────────────────────────────────────");
    printf("%s\n", " 4 │ Back");
    printf("%s\n", "═══╩════════════════════════════════════════════════════════════════════════════");

    printf("%s", "Enter Choice: ");
    scanf("%d", &choice);

    system("clear");
    switch(choice){
        case 1:
            show_table("expense");
            expense_manager();
            break;
        case 2:
            add_expense();
            expense_manager();
            break;
        case 3:
            show_table("expense");

            printf("%s", "Enter ID: ");
            scanf("%s", id);

            remove_expense(id);
            expense_manager();
            break;
        case 4:
            draw_main();
            break;
        default:
            expense_manager();
            break;
    }
}
