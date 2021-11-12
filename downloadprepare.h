#ifndef DOWNLOADPREPARE_H
#define DOWNLOADPREPARE_H

#include <QObject>
#include <QDir>
#include <QFile>
#include "networkoperate.h"


//下载插件之前的一些准备工作

class DownLoadPlugins: public QObject
{
    Q_OBJECT
public:
    DownLoadPlugins(QObject* parents=nullptr);
    ~DownLoadPlugins();

    void getDependPlugin(QMap<QString, QStringList>& pluginDependencyList, QSet<QString>& allPugins, QStringList roots);

    QJsonObject readJsonFile(const QString& path);

    QStringList toStringList(const QJsonArray& arr);

    QStringList getFileNames(const QString& path);

    QString getEnviroment();

    QString getVersion(int index);

    QString getPluginName(int index);

    QList<QJsonObject> getProResouceArr();

    QMap<QString, QJsonObject> getResources();

signals:
    void signalCreateTable(int rows, int columns);

//    void getPlugin(pulginName, gitURl, path);
public slots:
    void getDependPluginList(QStringList specifivPlugin={});

private:
    QSharedPointer<NetworkOperate> NetOp;
    QMap<QString, QJsonObject> pro_resources;
    QList<QJsonObject> pro_resource_arr;
    QString m_env;
};

#endif // DOWNLOADPREPARE_H
