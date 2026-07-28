#include "HeaderImage.h"

#include "AssetPath.h"
#include "env.h"

#include <QPainter>
#include <QtMath>

HeaderImage::HeaderImage(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMinimumWidth(0);
    loadImage(":/App/header.png");

    installEventFilter(this);
}

int HeaderImage::heightForWidth(int width) const
{
    if (m_aspectRatio <= 0.0 || width <= 0) {
        return 0;
    }

    return qMax(1, qRound(static_cast<double>(width) / m_aspectRatio));
}

bool HeaderImage::hasImage() const
{
    return !m_source.isNull();
}

QSize HeaderImage::sizeHint() const
{
    const int w = width() > 0 ? width() : 320;
    return QSize(w, heightForWidth(w));
}

QSize HeaderImage::minimumSizeHint() const
{
    return QSize(0, 0);
}

void HeaderImage::loadImage(const QString &path)
{
    if (!m_source.load(path)) {
        return;
    }

    if (m_source.height() > 0) {
        m_aspectRatio = static_cast<double>(m_source.width()) / m_source.height();
    }

    updateGeometry();
    update();
}

void HeaderImage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (m_source.isNull()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawPixmap(rect(),
                       m_source.scaled(size(),
                                       Qt::IgnoreAspectRatio,
                                       Qt::SmoothTransformation));
}

bool HeaderImage::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == this && e->type() == QEvent::ContextMenu){
        qDebug()<<"ToDo - Header - ContextMenu";
        return true;
    }
    return false;
}
