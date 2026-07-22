#ifndef HEADERDOCKPANEL_H
#define HEADERDOCKPANEL_H

#include <QWidget>

class HeaderImage;
class VerticalDockBox;

class HeaderDockPanel : public QWidget
{
    Q_OBJECT

public:
    explicit HeaderDockPanel(QWidget *parent = nullptr);
    VerticalDockBox * getDockBox(){
        return this->m_body;
    };

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void layoutChildren();

    HeaderImage *m_header = nullptr;
    VerticalDockBox *m_body = nullptr;
};

#endif // HEADERDOCKPANEL_H
