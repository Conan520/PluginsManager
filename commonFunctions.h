#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <QString>
#include <QProcess>
#include <QException>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QMap>
#include <QDebug>
#include <QByteArray>
#include <private/qzipwriter_p.h>
#include <private/qzipreader_p.h>

struct PluginInfos {
    bool hasOldVerison;
    bool isGitRepo;
    QString oldVersionPath;
};


struct Manifest {
    QString pluginName;
    bool isSource;
    QString commitHash;
    QString buildTime;
};

struct ExistAndMainfest{
    bool isPackageExist;
    Manifest manifest;
};

const QMap<QString, QString> branches = {
    {"Dev", "dev"},
    {"Test", "test"},
    {"Pre", "master"},
    {"Online", "master"},
    {"Meta", "meta"},
};


QString executeCmd(QString cmd);

// 获取某个路径的所有文件夹
QStringList getDirectories(QString path);

void getFiles(QString path, QVector<QString> &fileLists);

// 解压文件夹
void zipReader(QString filePath);

bool copyFolder(const QString &srcFilePath, const QString &tgtFilePath);

bool removeFolder(const QString& path);

#endif // COMMONFUNCTIONS_H
