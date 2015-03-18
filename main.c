#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

void draw_main();
void add_expense();
void add_remove_user();

void add_expense() {
    int choice;
    printf("%s\n", "DONE");
    draw_main();
}

void add_remove_user() {
    int choice;
    //clrscr();
    printf("%s\n", "DONE");

    scanf("%d", &choice);

    switch(choice) {
        case 1:
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
    printf("%s\n", "1. Add/Remove User");
    printf("%s\n", "2. Add Expense");
    printf("%s\n", "0. exit");

    scanf("%d", &choice);

    switch(choice) {
        case 0:
            return;
            break;
        case 1:
            add_remove_user();
            break;
        case 2:
            add_expense();
            break;
        default:
            draw_main();
            break;
    }

}

int main(void) {

    sqlite3 *db;

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
