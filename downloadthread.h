#ifndef DOWNLOADTHREAD_H
#define DOWNLOADTHREAD_H

#include <QThread>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QVector>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>
#include "commonFunctions.h"
#include <QApplication>
class DownloadThread: public QThread
{
    Q_OBJECT
public:
    DownloadThread(QUrl url, QObject* parents=nullptr);
    DownloadThread(QString pluginName, QString gitUrl, QString OSSPath, QString gitCommit, QString verison, QString env, qint64 file_size);
    ~DownloadThread();

    void getPlugin();

    bool checkGitAccess(QString gitUrl);

    bool checkIsGitRepo(QString path);

    QString getCurrentBranchName(QString path=".");

    PluginInfos checkOldVersion(QString pluginName);

    void copyGitHook();

    Manifest makeManifest(QString pluginName, bool isSource, QString gitCommit, QString version);

    QString getCommitSha1(QString path=".");

    void setNewPath(QString pathName, QString pluginName);

    ExistAndMainfest getOssPackage(QString ossPath, QString pluginName, QString verison, QString gitCommit);

    // 实际下载的函数
    void Download(QUrl url);

    QString setPath(QString pluginName);

    QString gitClone(QString url, QString pluginName, QString branch, QString gitCommit, bool forcePull);


signals:
    void over();
    void setProgressValue(qint64 bytesRead, qint64 totalBytes);     // 操作进度条的函数
    void sigResetProgressBar();     // 重新进度条的信号，发送给主线程
    void sig_decompress(QString filePath);
    void afterDecompress();

protected:
    void run();

public slots:
    void pluginDownloader();
    void setProgressBar(qint64 bytesRead, qint64 totalBytes);   // 发射设置进度条的信号
    void resetProBar(); //重置进度条
    void decompressFile(QString filePath);
    void onReadyRead();

private:
    // 要下载的插件信息
    QString m_pluginName;
    QString m_gitUrl;
    QString m_OSSPath;
    QString m_gitCommit;
    QString m_version;
    QString m_Env;
    qint64 fileSize;
    qint64 bytesRead = 0;
    // 有关网络操作的成员变量
    QUrl m_url;
    QNetworkAccessManager* m_NetManager;
    QNetworkReply *m_reply;
    QFile* file;
};


#endif // DOWNLOADTHREAD_H
