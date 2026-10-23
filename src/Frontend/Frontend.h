#pragma once

#include <deque>
#include <functional>

#include "../Backend/NetMessage.h"

class Fl_Button;
class Fl_Widget;
class DashboardWindow;
class MessageTable;
class Backend;

class Frontend
{
public:
    Frontend();
    void update();
    bool attachBackend(Backend* backend);

private:
    static void handleClear(Fl_Widget* widget, void* data); //params and static necessary for FLTK callback signature
    void clearDisplayedMessages();

    DashboardWindow* window;
    MessageTable* table;
    Fl_Button* clearButton;
    Backend* backend;

};
