#ifndef ITEMFRAME_H
#define ITEMFRAME_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

#include "../Theme.h"

class ItemFrame : public QWidget
{
    Q_OBJECT
public:
    explicit ItemFrame(QString title, QWidget *content, QWidget *parent = nullptr);
    void setTheme(Theme theme);

private:
    bool eventFilter(QObject *obj, QEvent *e);

private:
    QVBoxLayout *m_root_layout, *m_layout;
    QWidget *m_main_widget, *m_content;
    QLabel *m_header_label;

    Theme theme;

signals:
};

#endif // ITEMFRAME_H
