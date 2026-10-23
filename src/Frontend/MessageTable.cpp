#include "MessageTable.h"

#include <algorithm>
#include <cmath>

#include <FL/Fl.H>
#include <FL/fl_draw.H>

namespace
{
constexpr int tableCellPadding = 8;
constexpr int tableFontSize = 14;
constexpr int tableBaseMessageWidth = 600;
constexpr int tableMaximumAppWidth = 300;
constexpr int tableBaseAppWidth = 120;

int textWidthInPixels(const char* text)
{
    return static_cast<int>(std::ceil(fl_width(text)));
}
}

MessageTable::MessageTable(int x, int y, int w, int h)
    : Fl_Table(x, y, w, h)
{
    fl_font(FL_HELVETICA, tableFontSize);
    timestampWidth = textWidthInPixels("0000-00-00 00:00:00") + (2 * tableCellPadding);
    appWidth = tableBaseAppWidth;

    cols(3);
    col_header(1);
    col_header_height(28);
    row_header(0);
    row_height_all(28);
    col_resize(1);
    col_width(0, timestampWidth);
    end();
    updateMessageWidth();
}

void MessageTable::setMessages(const std::deque<NetMessage>& messages)
{
    displayMessages.clear();
    displayMessages.reserve(messages.size());
    fl_font(FL_HELVETICA, tableFontSize);
    appWidth = tableBaseAppWidth;

    for (const NetMessage& message : messages)
    {
        displayMessages.push_back({
            message.getHumanReadableTimestamp(),
            message.getApp(),
            message.getMessage()
        });
        appWidth = std::max(textWidthInPixels(displayMessages.back().app.c_str()) + (2 * tableCellPadding), appWidth); //Keep longest app name width
        messageWidth = std::max(textWidthInPixels(displayMessages.back().message.c_str()) + (2 * tableCellPadding), messageWidth); //Keep longest message width
    }

    rows(static_cast<int>(displayMessages.size()));
    updateMessageWidth();
    redraw();
}

void MessageTable::clearMessages()
{
    displayMessages.clear();
    fl_font(FL_HELVETICA, tableFontSize);
    appWidth = tableBaseAppWidth;
    messageWidth = tableBaseMessageWidth;
    rows(0);
    updateMessageWidth();
    redraw();
}

void MessageTable::resize(int x, int y, int w, int h)
{
    Fl_Table::resize(x, y, w, h);
    updateMessageWidth();
}

void MessageTable::draw_cell(
    TableContext context,
    int row,
    int column,
    int x,
    int y,
    int w,
    int h)
{
    if (context == CONTEXT_COL_HEADER)
    {
        static const char* headers[] = {"Timestamp", "App", "Message"};
        draw_box(FL_THIN_UP_BOX, x, y, w, h, FL_BACKGROUND_COLOR);
        fl_color(FL_FOREGROUND_COLOR);
        fl_font(FL_HELVETICA_BOLD, tableFontSize);
        fl_draw(headers[column], x + tableCellPadding, y, w - (2 * tableCellPadding), h, FL_ALIGN_LEFT);
        return;
    }

    if (context != CONTEXT_CELL || row < 0 ||
        static_cast<size_t>(row) >= displayMessages.size())
    {
        return;
    }

    const Fl_Color background = row % 2 == 0 ? FL_BACKGROUND_COLOR : FL_BACKGROUND2_COLOR; //Alternate colors in rows
    draw_box(FL_FLAT_BOX, x, y, w, h, background);
    fl_color(FL_FOREGROUND_COLOR);
    fl_font(FL_HELVETICA, tableFontSize);

    const DisplayMessage& message = displayMessages[static_cast<size_t>(row)];
    std::string value;
    switch (column)
    {
    case 0:
        value = message.timestamp;
        break;
    case 1:
        value = message.app;
        break;
    case 2:
        value = message.message;
        break;
    default:
        return;
    }

    fl_push_clip(x + tableCellPadding, y, std::max(0, w - (2 * tableCellPadding)), h);
    fl_draw(value.c_str(), x + tableCellPadding, y, w - (2 * tableCellPadding), h,
            FL_ALIGN_LEFT | FL_ALIGN_CLIP);
    fl_pop_clip();
}

void MessageTable::updateMessageWidth()
{
    int tempWidth = std::min(std::max(appWidth, tableBaseAppWidth), tableMaximumAppWidth);
    col_width(1, tempWidth);
    
    const int scrollbarWidth = vscrollbar->visible() ? Fl::scrollbar_size() : 0;
    const int availableWidth = w() - timestampWidth - tempWidth -
        scrollbarWidth - 2;
    col_width(2, std::max(availableWidth, messageWidth));
}