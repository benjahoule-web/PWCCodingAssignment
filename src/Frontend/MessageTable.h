#pragma once

#include <deque>
#include <string>
#include <vector>

#include <FL/Fl_Table.H>

#include "../Backend/NetMessage.h"

class MessageTable : public Fl_Table
{
public:
    MessageTable(int x, int y, int w, int h);

    void setMessages(const std::deque<NetMessage>& messages);
    void clearMessages();
    void resize(int x, int y, int w, int h) override;

protected:
    void draw_cell(TableContext context, int row, int column, int x, int y, int w, int h) override;

private:
    struct DisplayMessage
    {
        std::string timestamp;
        std::string app;
        std::string message;
    };

    void updateMessageWidth();

    std::vector<DisplayMessage> displayMessages;
    int timestampWidth = 0;
    int appWidth = 0;
    int messageWidth = 0;
};
