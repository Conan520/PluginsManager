#ifndef NETWORKOPERATE_H
#define NETWORKOPERATE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QFile>


class NetworkOperate : public QObject
{
    Q_OBJECT
public:
//    friend class DownLoadPlugins;
    ~NetworkOperate();
    QJsonObject getResources();
    QJsonArray getResourcesArr();
    QString getPluginName(const QJsonArray &arr, int index);
    // 获取resources_arr中指定索引的版本
    QString getVersion(int index);
    // 将resource_arr转换为QMap
    QMap<QString, QJsonObject> getResourceMap();


    static QSharedPointer<NetworkOperate> Create();

private slots:
    void finishedSlot(QNetworkReply*);

private:
    QNetworkAccessManager *m_NetManager;
    QNetworkReply* m_Reply;
    QJsonObject resources;
    QJsonArray resources_arr;
    QMap<QString, QJsonObject> resource_map;
    NetworkOperate(QObject *parent = nullptr);
    static QSharedPointer<NetworkOperate> NetOperate;

signals:
    void finished(QStringList);
};

#endif // NETWORKOPERATE_H
