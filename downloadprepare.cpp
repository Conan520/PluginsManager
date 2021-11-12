#include "downloadprepare.h"

DownLoadPlugins::DownLoadPlugins(QObject *parents): QObject(parents)
{
    NetOp = NetworkOperate::Create();
}

DownLoadPlugins::~DownLoadPlugins()
{
//    delete NetOp;
}

QStringList DownLoadPlugins::toStringList(const QJsonArray &arr)
{
    QStringList result;
    for (const auto& str: arr) {
        result.append(str.toString());
    }
    return result;
}

QStringList DownLoadPlugins::getFileNames(const QString &path)
{
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    qDebug() << files;
    return files;
}

QString DownLoadPlugins::getEnviroment()
{
    return this->m_env;
}

QString DownLoadPlugins::getVersion(int index)
{
    const int size = pro_resource_arr.size();
    if (size != 0)
    {
        if (index < 0 || index >= size)
        {
            qDebug() << "index invalid, please make sure the index of arr is valid";\
            return "";
        }
        else
        {
            QString version = pro_resource_arr.at(index).value("version").toString();
            return version;
        }
    }
    else
    {
        qDebug() << "the size of resources_arr is zero";
        return "";
    }
}

QString DownLoadPlugins::getPluginName(int index)
{
    const int size = pro_resource_arr.size();
    if (size != 0)
    {
        if (index < 0 || index >= size)
        {
            qDebug() << "index invalid, please make sure the index of arr is valid";
            return "";
        }
        else
        {
            QString PluginName = pro_resource_arr.at(index).value("name").toString();
            return PluginName;
        }
    }
    else {
        qDebug() << "the size of pro_resources_arr is zero";
        return "";
    }
}

QList<QJsonObject> DownLoadPlugins::getProResouceArr()
{
    return this->pro_resource_arr;
}

QMap<QString, QJsonObject> DownLoadPlugins::getResources()
{
    return pro_resources;
}

void DownLoadPlugins::getDependPlugin(QMap<QString, QStringList> &pluginDependencyList, QSet<QString> &allDenpendences, QStringList parents)
{
    QStringList children;
    for(auto& plugin: parents)
    {
        QList<QString> keys = pluginDependencyList.keys();
        if (keys.contains(plugin))
        {
            for (auto& child: pluginDependencyList.value(plugin))
            {
                allDenpendences.insert(child);
                children.append(child);
            }
        }
    }
    if (!children.isEmpty())
    {
        getDependPlugin(pluginDependencyList, allDenpendences, children);
    }
}

// 返回的是插件名和有关插件信息的map
void DownLoadPlugins::getDependPluginList(QStringList specifivPlugin)
{
    QMap<QString, QJsonObject> resourceMap = NetOp->getResourceMap();
    QMap<QString, QStringList> pluginDependencyList;
    if (!specifivPlugin.isEmpty())
    {
        QList<QString> keys = resourceMap.keys();
        for (auto& key: keys)
        {
            if (!specifivPlugin.contains(key)) {
                resourceMap.remove(key);
            }
        }
    }

    QMapIterator<QString, QJsonObject> iter(resourceMap);
    while (iter.hasNext()) {
        iter.next();
        QJsonArray arr = iter.value().value("dependency").toArray();
        pluginDependencyList.insert(iter.key(), toStringList(arr));
    }

    QSet<QString> allPlugins;
    QStringList roots = {};
    QString env = "Pre";

    if (specifivPlugin.isEmpty())
    {
        // 获取当前目录下的所有文件名
        if (getFileNames("D:/gitcloneProjects/metaworldeditor/").contains("roots_config"))
        {
            QJsonObject fileData = readJsonFile("D:/gitcloneProjects/metaworldeditor/roots_config");
            roots = toStringList(fileData.value("roots").toArray());
            qDebug() << roots;
            env = fileData.value("environment").toString();
            m_env = env;
        }
        if (!roots.isEmpty())
        {
            for (auto& root: roots) {
                allPlugins.insert(root);
            }

            getDependPlugin(pluginDependencyList, allPlugins, roots);

            QList<QString> keys = resourceMap.keys();
            for (auto& key: keys) {
                if (!allPlugins.contains(key)) {
                    resourceMap.remove(key);
                }
            }
            qDebug() << "allPlugins.size : " << allPlugins.size() << "\n" << allPlugins;
            qDebug() << "resourcesMap:" << resourceMap.size() << "\n" << resourceMap.keys();
            pro_resource_arr = resourceMap.values();
            pro_resources = resourceMap;
        }
    }
    emit signalCreateTable(resourceMap.size(), 5);
}

QJsonObject DownLoadPlugins::readJsonFile(const QString &path)
{
    QFile file(path);
    QJsonObject res;
    if (!file.exists()) {
        qDebug() << "the file not exists, please checkout the file path";
        return res;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "the file cannot open";
        return res;
    }
    QByteArray data = file.readAll();
    QJsonParseError err;
    QJsonDocument  data_doc = QJsonDocument::fromJson(data, &err);//字符串格式化为JSON

    if(err.error != QJsonParseError::NoError)
    {
        qDebug() << " JSON format error:" << err.errorString();
    }
    else {
        res = data_doc.object();
    }
    return res;
}


