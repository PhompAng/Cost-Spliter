#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

void draw_main();
void add_expense();
void add_remove_user();
int callback(void *, int, char **, char **);
sqlite3 *db;

int callback(void *NotUsed, int argc, char **argv,
                    char **azColName) {

    NotUsed = 0;

    for (int i = 0; i < argc; i++) {
        printf("%10.10s|", argv[i] ? argv[i] : "NULL");
    }

    printf("\n");

    return 0;
}

void add_expense() {
    int choice;
    printf("%s\n", "DONE");
    draw_main();
}

void add_remove_user() {
    int choice;
    char name[999];

    char *err_msg = 0;
    char sql[999] = " ";
    int rc;

    printf("%s\n", "1. Show User");
    printf("%s\n", "2. Add User");
    printf("%s\n", "3. Back");

    printf("%s", "Enter choice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            system("clear");
            printf("=================================\n");
            printf("%10s|%10s|%10s|\n", "ID", "Username", "Owe");
            rc = sqlite3_exec(db, "SELECT * FROM user", callback, 0, &err_msg);
            if (rc != SQLITE_OK ) {

                fprintf(stderr, "Failed to select data\n");
                fprintf(stderr, "SQL error: %s\n", err_msg);

                sqlite3_free(err_msg);
                sqlite3_close(db);

                exit(1);
            }
            printf("=================================\n");
            add_remove_user();
            break;
        case 2:
            printf("%s", "Enter Username: ");
            scanf("%s", name);
            sprintf(sql, "INSERT INTO user(name, owe) VALUES ('%s', 0);", name);
            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
            if (rc != SQLITE_OK ) {
                printf("%s\n", "Failed to create table");
                printf("SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
            } else {
                printf("%s\n", "User added successfully");
            }
            system("clear");
            add_remove_user();
            break;
        case 3:
            draw_main();
            break;
        default:
            system("clear");
            add_remove_user();
            break;
    }

}

void draw_main() {
    int choice;

    //clrscr();
    system("clear");
    printf("%s\n", "1. Add/Remove User");
    printf("%s\n", "2. Add Expense");
    printf("%s\n", "0. exit");

    printf("%s", "Enter choice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 0:
            return;
            break;
        case 1:
            system("clear");
            add_remove_user();
            break;
        case 2:
            system("clear");
            add_expense();
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

    draw_main();
    sqlite3_close(db);
    printf("%s\n", "BYE");
    return 0;
}
