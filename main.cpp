#include "main_window.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    main_window w;
    w.show();

    return a.exec();
}

// TODO: Backup before saving
// TODO: Add popularity to items
// TODO: Save windows sizes and positions
// TODO: Update profit from values (or a button to update one or all profits)
// TODO: Change profit to "transactions"/pupularity
// TODO: Add more gold, platinum and crystal coins images (1-2-3-4-5-10-25-50)
// TODO: Auto-load NPC values from wiki
// TODO: Add last updated for offers
// TODO: Change offer spinboxes to red when 100 is reached
// TODO: Update popularity from values (or a a button...)
// TODO: Use same column sizes on buy/sell tables
// TODO: Add npc buy/sell values on tables
// TODO: Create offer/go to offer from report
// TODO: Highlight total fee when > than average profit
// TODO: Highlight offers when buy/sell price is higher/lower than max/min values
// TODO: Add a temporary offers log
// TODO: Add create/edit/remove items on report
// TODO: Flag items with offers on reports
// TODO: Search fields for sell, buy and reports https://qt-project.org/doc/qt-4.7/qsortfilterproxymodel.html#filterAcceptsRow
// TODO: Most updated time/date/day
// TODO: Auto-copy on update mode
// TODO: Redo with ctrl-z
// TODO: Mru
// TODO: Shortcuts
// TODO: Add offer age
