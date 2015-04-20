#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "callback.h"
#include "show_data.h"
#include "user_manager.h"
#include "expense_manager.h"
#include "draw_main.h"

sqlite3 *db;

int main(void) {
    int rc = sqlite3_open("ljdata.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n",
        sqlite3_errmsg(db));
        sqlite3_close(db);

        exit(0);
    }

    system("clear");

    printf("%s\n", "════════════════════════════════════════════════════════════════════════════════");
    printf("%s\n", "                     ╔═╗┌─┐┌─┐┌┬┐   ╔═╗┌─┐┬  ┬┌┬┐┌┬┐┌─┐┬─┐");
    printf("%s\n", "                     ║  │ │└─┐ │ ───╚═╗├─┘│  │ │  │ ├┤ ├┬┘");
    printf("%s\n", "                     ╚═╝└─┘└─┘ ┴    ╚═╝┴  ┴─┘┴ ┴  ┴ └─┘┴└─");
    printf("%s\n", "════════════════════════════════════════════════════════════════════════════════");

    sleep(3);

    system("clear");
    draw_main();
    sqlite3_close(db);
    printf("%s\n", "BYE");
    system("clear");
    return 0;
}
