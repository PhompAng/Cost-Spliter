#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include "user_manager.h"
#include "draw_main.h"
#include "show_data.h"

sqlite3 *db;
sqlite3_stmt *stmt;

void alter_table(char table[], char name[]) {
    char *err_msg = 0;
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
    char *err_msg = 0;
    int rc;
    char sql[999] = " ";

    sprintf(sql, "INSERT INTO user(name) VALUES ('%s');", name);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    system("clear");
    if (rc != SQLITE_OK ) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        alter_table("spent_detail", name);
        alter_table("paid_detail", name);
        alter_table("balance_detail", name);
        printf("User %s added successfully\n", name);
    }
}

void remove_user(char id[]) {
    char *err_msg = 0;
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

    printf("%s\n", "═══╦════════════════════════════════════════════════════════════════════════════");
    printf("%s\n", " 1 │ Show User");
    printf("%s\n", " 2 │ Add User");
    printf("%s\n", " 3 │ Remove User");
    printf("%s\n", "───┼────────────────────────────────────────────────────────────────────────────");
    printf("%s\n", " 4 │ Back");
    printf("%s\n", "═══╩════════════════════════════════════════════════════════════════════════════");

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

            user_manager();
            break;
        case 3:
            show_table("user");

            printf("%s", "Enter ID: ");
            scanf("%s", id);

            remove_user(id);

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
