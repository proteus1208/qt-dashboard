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

    int gap = 4;
    QColor bg = QColor("#040726");

    int dock_padding = 2;
    int dock_gap = 4;
    QColor dock_bg = QColor("#1a1d3c");

    QColor dock_border = QColor("#222549");
    int dock_header_height = 42;
    QColor dock_header_bg = QColor("#303055");

    QColor dock_header_text_color = QColor("#ffffff");
    int dock_header_text_fontSize = 20;

    int dock_content_gap = 4;
    int dock_content_padding = 4;

    QColor dock_content_item_bg = QColor("#ffffff");
    QColor dock_content_item_header_bg = QColor("#303055");
    QColor dock_content_item_header_text_color = QColor("#ffffff");
    int dock_content_item_header_height = 24;

    int dock_content_item_header_text_fontSize = 18;

    QColor dock_content_item_content_bg = QColor("#26294a");
    QColor dock_content_item_content_text = QColor("#ffffff");

    // Split layout (VerticalDockBox / HorizentalLayout) theming.
    QColor split_bg = QColor(0x1e, 0x1e, 0x1e);
    QColor split_border = QColor(0x3d, 0x3d, 0x3d);
    int split_border_width = 2;
    QColor split_hover = QColor(0x00, 0x8c, 0xff);
    QColor split_preview = QColor("#0069D1");
};
Q_DECLARE_METATYPE(Theme);
#endif // THEME_H
