#include "Frontend.h"
#include "../Backend/Backend.h"

#include <utility>

#include <FL/Fl_Button.H>

#include "DashboardWindow.h"
#include "MessageTable.h"

constexpr int kWindowWidth = 900;
constexpr int kWindowHeight = 500;

Frontend::Frontend()
    : window(new DashboardWindow(kWindowWidth, kWindowHeight, "Observability Dashboard")),
      table(nullptr),
      clearButton(nullptr)
{
    table = new MessageTable(0, 0, 1, 1);

    clearButton = new Fl_Button(
        0,
        0,
        1,
        1,
        "Clear");
    clearButton->callback(handleClear, this); //We pass "this" as data so we can access members in the static callback

    window->end();
    window->setContents(table, clearButton);
    window->show();
}

bool Frontend::attachBackend(Backend* newBackend)
{
    if (!newBackend)
    {
        return false;
    }

    backend = newBackend;
    return true;
}

void Frontend::update()
{
    if (!backend)
    {
        return;
    }
    table->setMessages(backend->getNetMessages());
}

void Frontend::handleClear(Fl_Widget*, void* data)
{
    static_cast<Frontend*>(data)->clearDisplayedMessages();
}

void Frontend::clearDisplayedMessages()
{
    table->clearMessages();
    if (backend)
    {
        backend->flushRecentMessages();
    }
}
