#ifndef ASSETPATH_H
#define ASSETPATH_H

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>

inline QString resolveAssetPath(const QString &relativePath)
{
    const QStringList searchRoots = {
        QCoreApplication::applicationDirPath(),
        QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("..")),
        QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("../..")),
        QDir::currentPath(),
    };

    for (const QString &root : searchRoots) {
        const QString candidate = QDir(root).filePath(relativePath);
        if (QFileInfo::exists(candidate)) {
            return candidate;
        }
    }

    return relativePath;
}

#endif // ASSETPATH_H
