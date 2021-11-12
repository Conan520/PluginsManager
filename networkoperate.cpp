#include "networkoperate.h"
#include <QUrl>
#include <QVariant>

QSharedPointer<NetworkOperate> NetworkOperate::NetOperate = nullptr;

NetworkOperate::NetworkOperate(QObject *parent) : QObject(parent)
{
    m_NetManager = new QNetworkAccessManager();
    QUrl url("http://test-plugins.meta-verse.co/Plugin/List");
    m_Reply = m_NetManager->get(QNetworkRequest(url));
    connect(m_NetManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
}

NetworkOperate::~NetworkOperate()
{
    delete m_NetManager;
}

QJsonObject NetworkOperate::getResources()
{
    return this->resources;
}

QJsonArray NetworkOperate::getResourcesArr()
{
    return this->resources_arr;
}

QString NetworkOperate::getPluginName(const QJsonArray &arr, int index)
{
    if (arr.size() != 0)
    {
        if (index < 0 || index >= arr.size())
        {
            qDebug() << "index invalid, please make sure the index of arr is valid";
            return "";
        }
        else
        {
            QJsonObject temp = arr[index].toObject();
            QString PluginName = temp.value("name").toString();
            return PluginName;
        }
    }
    else {
        qDebug() << "the size of resources_arr is zero";
        return "";
    }
}

QString NetworkOperate::getVersion(int index)
{
    const int size = this->resources_arr.size();
    if (size != 0)
    {
        if (index < 0 || index >= size)
        {
            qDebug() << "index invalid, please make sure the index of arr is valid";\
            return "";
        }
        else
        {
            QString version = resources_arr[index].toObject().value("sha1").toString();
            return version;
        }
    }
    else
    {
        qDebug() << "the size of resources_arr is zero";
        return "";
    }
}

QMap<QString, QJsonObject> NetworkOperate::getResourceMap()
{
    if (resource_map.isEmpty())
    {
        if (!resources_arr.isEmpty())
        {
            for (int i = 0; i < resources_arr.size(); i++)
            {
                QJsonObject value = resources_arr[i].toObject();
                QString name = resources_arr[i].toObject().value("name").toString();
                resource_map.insert(name, value);
            }
        }
    }
    return resource_map;    //有可能返回空Map
}

QSharedPointer<NetworkOperate> NetworkOperate::Create()
{
    if (!NetOperate)
    {
        NetOperate = QSharedPointer<NetworkOperate>(new NetworkOperate);
    }
    return NetOperate;
}

void NetworkOperate::finishedSlot(QNetworkReply *)
{
    m_Reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    m_Reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if(m_Reply->error() == QNetworkReply::NoError)
    {
        const QByteArray bytes = m_Reply->readAll();
        QJsonParseError err;
        QJsonDocument  dataDoc = QJsonDocument::fromJson(bytes, &err);//字符串格式化为JSON

        if(err.error != QJsonParseError::NoError)
        {
            qDebug() << "JSON格式错误:" << err.errorString();
            return;
        }
        else
        {
            // qDebug() << "JSON格式正确：\n" << root_Doc << "\n";
            resources = dataDoc.object();
            resources_arr = resources.value("resources").toArray();
            for (int i = 0; i < resources_arr.size(); i++)
            {
                QJsonObject value = resources_arr[i].toObject();
                QString name = resources_arr[i].toObject().value("name").toString();
                resource_map.insert(name, value);
            }
            qDebug() << resource_map.size();
            emit finished({});
        }
    }
    else
    {
        qDebug() <<"网络请求响应错误:" << m_Reply->errorString();
    }
}
