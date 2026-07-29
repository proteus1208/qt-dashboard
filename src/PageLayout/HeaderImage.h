#ifndef HEADERIMAGE_H
#define HEADERIMAGE_H

#include <QPixmap>
#include <QWidget>

class HeaderImage : public QWidget
{
    Q_OBJECT

public:
    explicit HeaderImage(QWidget *parent = nullptr);

    int heightForWidth(int width) const;
    bool hasImage() const;

protected:
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void paintEvent(QPaintEvent *event) override;

    bool eventFilter(QObject *obj, QEvent *e) override;

private:
    void loadImage(const QString &path);

    QPixmap m_source;
    double m_aspectRatio = 0.0;

signals:
    void contextMenuRequested(QPoint globalPos);
};

#endif // HEADERIMAGE_H
