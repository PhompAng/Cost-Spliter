#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

void draw_main();
void expense_manager();
void show_table(char[]);
//void show_user();
void add_remove_user();
int callback(void *, int, char **, char **);
sqlite3 *db;

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;

    for (int i = 0; i < argc; i++) {
        printf("%10.10s|", argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void expense_manager() {
    int choice;
    char name[999];
    int amount;
    char datetime[999];
    int number_of_splitter;
    int payer_id;
    int splitter_id;

    char *err_msg = 0;
    char sql[999] = " ";
    int rc;

    printf("%s\n", "1. Show Expense");
    printf("%s\n", "2. Add Expense");
    printf("%s\n", "3. Back");

    printf("%s", "Enter Choice: ");
    scanf("%d", &choice);

    switch(choice){
        case 1:
            printf("=================================\n");
            printf("%10s|%10s|%10s|%10s|\n", "ID", "Name", "Amount", "Time");
            show_table("expense");
            expense_manager();
            break;
        case 2:
            //TODO add_expense();
            printf("%s", "Expense Name: ");
            scanf("%s", name);
            printf("%s", "Amount: ");
            scanf("%d", &amount);
            printf("%s", "Time: ");
            scanf("%s", datetime);
            printf("%s", "Number of Splitter: ");
            scanf("%d", &number_of_splitter);
            printf("%s", "Payer ID: ");
            scanf("%d", &payer_id);
            for (int i = 0; i < number_of_splitter; ++i)
            {
                printf("%s", "Splitter ID: ");
                scanf("%d", &splitter_id);
            }
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
/*
void show_user() {
    int rc;
    char *err_msg = 0;
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
}
*/
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
            printf("%10s|%10s|%10s|\n", "ID", "Username", "Owe");
            show_table("user");
            add_remove_user();
            break;
        case 2:
            printf("%s", "Enter Username: ");
            scanf("%s", name);

            sprintf(sql, "INSERT INTO user(name, owe) VALUES ('%s', 0);", name);
            rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
            system("clear");
            if (rc != SQLITE_OK ) {
                printf("SQL error: %s\n", err_msg);
                sqlite3_free(err_msg);
            } else {
                printf("User %s added successfully\n", name);
            }
            printf("=================================\n");
            add_remove_user();
            break;
        case 3:
            printf("=================================\n");
            printf("%10s|%10s|%10s|\n", "ID", "Username", "Owe");
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
            expense_manager();
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
