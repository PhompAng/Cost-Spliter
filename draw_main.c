#include <stdio.h>
#include <stdlib.h>
#include "draw_main.h"
#include "expense_manager.h"
#include "user_manager.h"
#include "show_data.h"

void draw_main() {
    int choice;

    printf("%s\n", "═══╦════════════════════════════════════════════════════════════════════════════");
    printf("%s\n", " 1 │ User Manager");
    printf("%s\n", " 2 │ Expense Manager");
    printf("%s\n", " 3 │ Show Balance");
    printf("%s\n", "───┼────────────────────────────────────────────────────────────────────────────");
    printf("%s\n", " 0 │ Exit");
    printf("%s\n", "═══╩════════════════════════════════════════════════════════════════════════════");

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
