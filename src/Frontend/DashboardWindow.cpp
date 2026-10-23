#include "DashboardWindow.h"
#include <FL/Fl_Button.H>
#include "MessageTable.h"

namespace
{
constexpr int windowPadding = 10;
constexpr int buttonWidth = 90;
constexpr int buttonHeight = 30;
constexpr int buttonBottomPadding = 10;
}

DashboardWindow::DashboardWindow(int width, int height, const char* title)
    : Fl_Double_Window(width, height, title) 
{
    resizable(this);
}

void DashboardWindow::setContents(MessageTable* messageTable, Fl_Button* button)
{
    table = messageTable;
    clearButton = button;
    layoutContents(w(), h());
}

void DashboardWindow::resize(int x, int y, int width, int height)
{
    Fl_Double_Window::resize(x, y, width, height);
    layoutContents(width, height);
}

void DashboardWindow::layoutContents(int width, int height)
{
    if (table == nullptr || clearButton == nullptr)
    {
        return;
    }

    table->resize(
        windowPadding,
        windowPadding,
        width - (2 * windowPadding),
        height - buttonHeight - (3 * windowPadding));
    clearButton->resize(
        width - windowPadding - buttonWidth,
        height - buttonBottomPadding - buttonHeight,
        buttonWidth,
        buttonHeight);
}
