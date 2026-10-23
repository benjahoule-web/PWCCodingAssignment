#pragma once

#include <FL/Fl_Double_Window.H>

class Fl_Button;
class MessageTable;

class DashboardWindow : public Fl_Double_Window
{
public:
    DashboardWindow(int width, int height, const char* title);

    void setContents(MessageTable* messageTable, Fl_Button* button);
    void resize(int x, int y, int width, int height) override;

private:
    void layoutContents(int width, int height);

    MessageTable* table = nullptr;
    Fl_Button* clearButton = nullptr;
};
