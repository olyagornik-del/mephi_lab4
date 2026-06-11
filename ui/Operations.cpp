#include "menu.h"
#include "scanValues.h"
#include "sections.h"

// Главный цикл: показывает меню и раздаёт работу разделам.
void RunMenu() {
    printBanner();
    while (true) {
        printMainMenu();
        int choice = scanIntInRange(0, 5);
        switch (choice) {
            case 1: RunLazySection(); break;
            case 2: RunOrdinalCalc(); break;
            case 3: RunStatsSection(); break;
            case 4: RunSortSection(); break;
            case 5: RunAutoDemo(); break;
            case 0: return;
        }
    }
}
