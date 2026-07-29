#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QEvent>

class Theme{

public:
    static const int EventType = QEvent::User + 1;
    Theme(){

    }
    Theme(QString name){
        this->name = name;
    }

    QString name = "Default";

    // Neutral "slate" surfaces with a single indigo accent, layered from
    // darkest (app/split background) to most elevated (item header), plus
    // a consistent 8px spacing scale for a calmer, less cramped grid.
    int gap = 8;
    QColor bg = QColor("#0b0d12");

    int dock_padding = 3;
    int dock_gap = 8;
    QColor dock_bg = QColor("#12141b");

    QColor dock_border = QColor("#262a36");
    int dock_header_height = 36;
    QColor dock_header_bg = QColor("#171922");

    QColor dock_header_text_color = QColor("#eef0f5");
    int dock_header_text_fontSize = 14;
    int dock_header_padding_left = 12;

    int dock_content_gap = 8;
    int dock_content_padding = 8;

    QColor dock_content_item_bg = QColor("#1a1d26");
    QColor dock_content_item_header_bg = QColor("#20232e");
    QColor dock_content_item_header_text_color = QColor("#c7cbd6");
    int dock_content_item_header_height = 28;

    int dock_content_item_header_text_fontSize = 12;
    int dock_content_item_header_padding_left = 8;

    QColor dock_content_item_content_bg = QColor("#12141b");
    QColor dock_content_item_content_text = QColor("#eef0f5");

    // Split layout (VerticalDockBox / HorizentalLayout) theming.
    QColor split_bg = QColor("#0b0d12");
    QColor split_border = QColor("#262a36");
    int split_border_width = 1;
    QColor split_hover = QColor("#5b8cff");
    QColor split_preview = QColor("#805b8cff");
};
Q_DECLARE_METATYPE(Theme);
#endif // THEME_H
