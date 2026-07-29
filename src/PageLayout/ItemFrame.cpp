#include "ItemFrame.h"
#include <QString>
#include <QCoreApplication>

ItemFrame::ItemFrame(QString title, QWidget *content, QWidget *parent) :
    QWidget(parent),
    m_content(content)
{
    m_root_layout = new QVBoxLayout(this);
    m_root_layout->setContentsMargins(0, 0, 0, 0);
    m_root_layout->setSpacing(0);

    m_main_widget = new QWidget(this);

    m_root_layout->addWidget(m_main_widget);

    m_layout = new QVBoxLayout(m_main_widget);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    m_header_label = new QLabel(m_main_widget);
    m_header_label->setText(title);

    content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_layout->addWidget(m_header_label);
    m_layout->addWidget(content, 1);
}

void ItemFrame::setTheme(Theme theme)
{
    this->theme = theme;
    m_main_widget->setStyleSheet(
        QString("background:%1;")
            .arg(theme.dock_content_item_bg.name(QColor::HexRgb))
        );
    m_header_label->setStyleSheet(
        QString("padding-left:%1px;background:%2; color:%3; font-size:%4px;")
            .arg(theme.dock_content_item_header_padding_left)
            .arg(theme.dock_content_item_header_bg.name(QColor::HexRgb))
            .arg(theme.dock_content_item_header_text_color.name(QColor::HexRgb))
            .arg(theme.dock_content_item_header_text_fontSize)
        );
    m_header_label->setFixedHeight(theme.dock_content_item_header_height);
}

bool ItemFrame::eventFilter(QObject *obj, QEvent *e)
{
    if(e->type() == Theme::EventType){
        setTheme(qApp->property("Theme").value<Theme>());
    }
    return QWidget::eventFilter(obj, e);
}
