#include "commonFunctions.h"

QString executeCmd(QString cmd)
{
    try {
        QString right_res = "";
        QString err_res = "";
        QProcess* p = new QProcess();
        p->start(cmd);
        bool isOK = p->waitForFinished();
        if (isOK) {
            QByteArray std_bytes = p->readAllStandardOutput();
            QByteArray err_bytes = p->readAllStandardError();
            err_res  = QString::fromLocal8Bit(err_bytes);
            right_res = QString::fromLocal8Bit(std_bytes);
        } else {
            err_res = cmd + " command execute error";
        }
        QString res = err_res.isEmpty() ? right_res: err_res;
//        qDebug() << "executeCmd" << res;
        return res.remove("\r").remove("\n");
    }  catch (QException *e) {
        QString res = QString::fromLocal8Bit(e->what());
        qDebug() << res;
        return res.remove("\r").remove("\n");
    }
}

// 获取一个文件夹下的所有目录
QStringList getDirectories(QString path)
{
    QDir dir(path);
    QStringList directories;
    if (!dir.exists()) {
        directories = QStringList("");
        return directories;
    }
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
    dir.setSorting(QDir::Name);
    directories = dir.entryList();
    return directories;
}

// 递归获取某个文件夹下的所有文件
void getFiles(QString path, QVector<QString> &fileLists)
{
    try {
        QDir dir(path);
        if (dir.exists())
        {
            dir.setFilter(QDir::Dirs |QDir::Files|QDir::NoDotAndDotDot);
            QFileInfoList infoList = dir.entryInfoList();

            foreach(QFileInfo DirOrFile, infoList) {
                if (DirOrFile.isDir()) {
                    getFiles(DirOrFile.absoluteFilePath(), fileLists);
                }
                else {
                    fileLists.append(DirOrFile.absoluteFilePath());
                }
            }
        }
        return;
    }  catch (QException* e) {
        qDebug() << "In getFiles() unknown error occurred";
    }
}

bool copyFolder(const QString &srcFilePath, const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        targetDir.cdUp();
        if (targetDir.mkdir(QFileInfo(tgtFilePath).fileName()))
            return false;
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames) {
            const QString newSrcFilePath
                    = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath
                    = tgtFilePath + QLatin1Char('/') + fileName;
            if (!copyFolder(newSrcFilePath, newTgtFilePath))
                return false;
        }
    } else {
        if (!QFile::copy(srcFilePath, tgtFilePath))
            return false;
    }
    return true;
}

bool removeFolder(const QString &path)
{
    try {
        QDir dir(path);
        if(dir.exists()) {
            return dir.removeRecursively();
        }
    }  catch (QException* e) {
        qDebug() << "an unknown error has occurred, deleted folder " + path + "failed";
    }
    return false;
}

void zipReader(QString filePath)
{
    QZipReader cZip(filePath);
    foreach(QZipReader::FileInfo item, cZip.fileInfoList())
    {
        qDebug() << item.filePath;
        if (item.isDir)
        {
            QDir d(item.filePath);
            if (!d.exists())
                d.mkpath(item.filePath);
        }

        if (item.isFile)
        {
            QString filepath = item.filePath;
            const int index = filepath.lastIndexOf("/");

            QDir dir(filepath.left(index));
            if (!dir.exists()) {
                QDir("./").mkpath(filepath.left(index));    // 需要注意当前路径是什么
            }
            QFile file(item.filePath);

            file.open(QFile::WriteOnly);
            file.write(cZip.fileData(item.filePath));
            file.close();
        }
    }
    cZip.close();
}
