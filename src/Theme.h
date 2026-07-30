#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QEvent>

class Theme
{

public:
    static const int EventType = QEvent::User + 1;

    Theme()
    {
    }

    Theme(QString name)
    {
        this->name = name;
    }

    QString name = "Professional";

    //
    // Global
    //
    int gap = 4;
    QColor bg = QColor("#0D1117");

    //
    // Dock
    //
    int dock_padding = 2;
    int dock_gap = 4;

    QColor dock_bg = QColor("#161B22");
    QColor dock_border = QColor("#2D333B");

    //
    // Dock Header
    //
    int dock_header_height = 30;
    QColor dock_header_bg = QColor("#1C2128");

    QColor dock_header_text_color = QColor("#F0F6FC");
    int dock_header_text_fontSize = 13;
    int dock_header_padding_left = 10;

    //
    // Dock Content
    //
    int dock_content_gap = 4;
    int dock_content_padding = 4;

    //
    // Widget
    //
    QColor dock_content_item_bg = QColor("#1B2129");

    //
    // Widget Header
    //
    QColor dock_content_item_header_bg = QColor("#222933");
    QColor dock_content_item_header_text_color = QColor("#D8DEE9");

    int dock_content_item_header_height = 24;
    int dock_content_item_header_text_fontSize = 11;
    int dock_content_item_header_padding_left = 8;

    //
    // Widget Content
    //
    QColor dock_content_item_content_bg = QColor("#181D25");
    QColor dock_content_item_content_text = QColor("#F2F5F8");

    //
    // Splitter
    //
    QColor split_bg = QColor("#0D1117");

    QColor split_border = QColor("#30363D");
    int split_border_width = 1;

    QColor split_hover = QColor("#58A6FF");
    QColor split_preview = QColor(88, 166, 255, 80);

    //
    // Optional Accent Colors
    //
    QColor accent = QColor("#58A6FF");
    QColor success = QColor("#3FB950");
    QColor warning = QColor("#D29922");
    QColor danger = QColor("#F85149");

    //
    // Optional Text Colors
    //
    QColor text_primary = QColor("#F0F6FC");
    QColor text_secondary = QColor("#8B949E");
    QColor text_disabled = QColor("#6E7681");

    //
    // Context Menu
    //
    QColor menu_bg = QColor("#1C2128");
    QColor menu_border = QColor("#30363D");
    QColor menu_text = QColor("#F0F6FC");
    QColor menu_hover_bg = QColor("#58A6FF");
    QColor menu_hover_text = QColor("#FFFFFF");
    int menu_radius = 8;
    int menu_item_radius = 4;

    // Every QMenu in the app should be built with
    // menu->setStyleSheet(theme.menuStyleSheet()) so right-click menus follow
    // whichever theme is active instead of falling back to native/OS style.
    QString menuStyleSheet() const
    {
        return QString(
            "QMenu {"
            "  background-color: %1;"
            "  color: %2;"
            "  border: 1px solid %3;"
            "  border-radius: %4px;"
            "  padding: 4px;"
            "}"
            "QMenu::item {"
            "  background-color: transparent;"
            "  padding: 6px 28px 6px 12px;"
            "  border-radius: %5px;"
            "}"
            "QMenu::item:selected {"
            "  background-color: %6;"
            "  color: %7;"
            "}"
            "QMenu::item:disabled {"
            "  color: %3;"
            "}"
            "QMenu::separator {"
            "  height: 1px;"
            "  background: %3;"
            "  margin: 4px 8px;"
            "}"
            "QMenu::indicator {"
            "  width: 14px;"
            "  height: 14px;"
            "}"
        ).arg(menu_bg.name(QColor::HexRgb))
         .arg(menu_text.name(QColor::HexRgb))
         .arg(menu_border.name(QColor::HexRgb))
         .arg(menu_radius)
         .arg(menu_item_radius)
         .arg(menu_hover_bg.name(QColor::HexRgb))
         .arg(menu_hover_text.name(QColor::HexRgb));
    }
};

Q_DECLARE_METATYPE(Theme);

#endif // THEME_H