#include "downloadthread.h"

DownloadThread::DownloadThread(QUrl url, QObject* parents): QThread(parents)
{
    m_url = url;

    moveToThread(this);
}

DownloadThread::DownloadThread(QString pluginName, QString gitUrl, QString OSSPath, QString gitCommit, QString verison, QString env, qint64 file_size)
{
    m_pluginName = pluginName;
    m_gitUrl = gitUrl;
    m_OSSPath = OSSPath;
    m_gitCommit = gitCommit;
    m_version = verison;
    m_Env = env;
    fileSize = file_size;
    moveToThread(this);     // 将线程类中的槽函数放线程空间中执行
}

DownloadThread::~DownloadThread()
{
    delete m_NetManager;
    delete m_reply;
    delete file;
}

bool DownloadThread::checkGitAccess(QString gitUrl)
{
    bool hasAccess = true;
    QString cmd = "git clone -b asdfasuiodwjef " + gitUrl;
    QString res = executeCmd(cmd);
    qDebug() << "checkGitAccess" << res;
    const QString access_msg = "Please make sure you have the correct access rights";
    if (res.contains(access_msg)) {
        qDebug() << "Error: git access denied";
        hasAccess = false;
    }
    return hasAccess;
}

bool DownloadThread::checkIsGitRepo(QString path)
{
    QStringList dirs = getDirectories(path);
    return dirs.contains(".git");
}

QString DownloadThread::getCurrentBranchName(QString path)
{
    QString cmd = QString("git -C %1 rev-parse --abbrev-ref HEAD").arg(path);
    QString branchName = executeCmd(cmd);
    return branchName;
}

PluginInfos DownloadThread::checkOldVersion(QString pluginName)
{
    QString curPath = QDir::currentPath();
    QString path = curPath + "/Plugins";
    QStringList pluginDirs = getDirectories(path);
    if (pluginDirs.contains(pluginName)) {
        bool isGitRepo = checkIsGitRepo(path + "/" + pluginName);
        QString oldPath = path + "/" + pluginName;
        return {true, isGitRepo, oldPath};
    }
    return {false, false, ""};
}

void DownloadThread::copyGitHook()
{
    QString localGitHookPath = ".git/hooks/pre-commit";
    QString srcGitHookPath = "tools/pre-commit";
    QFile localFile(localGitHookPath);
    QFile srcFile(srcGitHookPath);
    if (!localFile.exists() && srcFile.exists()) {
        try {
            bool copySucceed= QFile::copy(srcGitHookPath, localGitHookPath);
            if (!copySucceed) {
                qDebug() << "copy tools/pre-commit file failed!";
            }
        }  catch (QException* e) {
            qDebug() << QString::fromLocal8Bit(e->what());
        }
    }
}

Manifest DownloadThread::makeManifest(QString pluginName, bool isSource, QString gitCommit, QString version)
{
    return Manifest{pluginName, isSource, gitCommit, version};
}

QString DownloadThread::getCommitSha1(QString path)
{
    QString originPath = QDir::currentPath();
    QString hash = "";
    try {
        QDir::setCurrent(path);
        QString commitId = executeCmd("git rev-parse HEAD");
        hash = commitId;
    }  catch (QException* e) {
        qDebug() << "failed to get commit sha1 from " + QDir::currentPath()+ ". reason:" + QString::fromLocal8Bit(e->what());
    }
    QDir::setCurrent(originPath);
    return hash;
}

// 前提是已经在Plugins目录下
QString DownloadThread::setPath(QString pluginName)
{
    QString configFile = "path_config.ini";
    QString moduleName = "";

    QVector<QString> files;
    getFiles(pluginName, files);

    foreach(QString absFileName, files) {
        if (absFileName.contains(configFile))
        {
            QSharedPointer<QSettings> configIniRead = QSharedPointer<QSettings>(new QSettings(absFileName, QSettings::IniFormat));
            moduleName = configIniRead->value("/mainModule/mainModuleName").toString();
            break;
        }
    }

    if (!moduleName.isEmpty())
    {
        QString MainModulePath = moduleName;
        if (!QDir(MainModulePath).exists()) {
            QDir("./").mkpath(MainModulePath);
        }
        QString newPath = MainModulePath + "/" + pluginName;

        QString cmd = "robocopy " + pluginName + " " + newPath + " /e";     // window自带的文件拷贝命令
        executeCmd(cmd);
        QDir srcDir(pluginName);
        bool isDeleted = srcDir.removeRecursively();    //有可能删除不了，需要改成强制删除
        if (!isDeleted) {
            qDebug() << "deleted folder " + pluginName + " failed";
        }
        return newPath;
    }
    return "";
}


void DownloadThread::setNewPath(QString pathName, QString pluginName)
{

    QString configFile = "path_config.ini";
    QString moduleName = "";

    QVector<QString> files;
    getFiles(pathName, files);

    foreach(QString absFileName, files) {
        if (absFileName.contains(configFile))
        {
            QSharedPointer<QSettings> configIniRead = QSharedPointer<QSettings>(new QSettings(absFileName, QSettings::IniFormat));
            moduleName = configIniRead->value("/mainModule/mainModuleName").toString();
            break;
        }
    }

    if (moduleName != "")
    {
        QString MainModulePath = "Plugins/" + moduleName;
        if (!QDir(MainModulePath).exists()) {
            QDir("./").mkpath(MainModulePath);
        }
        QString newPath = MainModulePath + "/" + pluginName;

        if (pathName != newPath) {
            QString cmd = "robocopy " + pathName + " " + newPath + " /e";
            executeCmd(cmd);
            QDir srcDir(pathName);
            bool isDeleted = srcDir.removeRecursively();    //有可能删除不了，需要改成强制删除
            if (!isDeleted) {
                qDebug() << "deleted folder " + pathName + " failed";
            }
        }
    }
}

void DownloadThread::Download(QUrl url)
{
    QNetworkRequest request;
    m_url = url;
    request.setUrl(m_url);
//    QNetworkReply* m_reply;

//    if (m_reply != Q_NULLPTR){
//        m_reply->deleteLater();
//    }

    m_reply = m_NetManager->get(request);
//    m_reply->setReadBufferSize(4 * 1024*1024);
    qDebug() << "start get.";
//    connect(m_reply, SIGNAL(finished()),this, SLOT(pluginDownloader()));
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
//    connect(m_reply, &QNetworkReply::downloadProgress, this, &DownloadThread::setProgressValue);
    connect(m_reply, &QNetworkReply::finished, this, &DownloadThread::resetProBar);
}

void DownloadThread::onReadyRead()
{
    const QVariant redirection = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if(!redirection.isNull()) {
        const QUrl redirectedUrl = m_url.resolved(redirection.toUrl());
        qDebug()<<"redirectedUrl:"<<redirectedUrl.url();
        m_reply = m_NetManager->get(QNetworkRequest(redirectedUrl));
        Download(redirectedUrl);
        return;
    }
    QDir::setCurrent("Plugins");
    QString fileName = m_pluginName + ".zip";   // 压缩文件名
    file = new QFile(fileName);
    QByteArray bytes = m_reply->readAll();
    bytesRead += bytes.size();
    qDebug() << "Reading " << bytesRead << "/" << fileSize;

    if (!file->open(QIODevice::WriteOnly|QIODevice::Append)){}
    file->write(bytes);
    file->flush();
    emit setProgressValue(bytesRead, fileSize);
    if (bytesRead == fileSize) {
        file->close();
        delete file;
        file = nullptr;
        emit sig_decompress(fileName);
        emit over();
        bytesRead = 0;
    }
}

void DownloadThread::pluginDownloader()
{
    file->flush();
    file->close();
//    qDebug() << "fileDownloader()() Thread id: " << QThread::currentThread();
//    const QVariant redirection = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

//    if(!redirection.isNull()) {
//        const QUrl redirectedUrl = m_url.resolved(redirection.toUrl());
//        qDebug()<<"redirectedUrl:"<<redirectedUrl.url();
//        m_reply = m_NetManager->get(QNetworkRequest(redirectedUrl));
//        Download(redirectedUrl);
//        return;
//    }
//    QDir::setCurrent("Plugins");
//    QByteArray bytes = m_reply->readAll();

//    qDebug() << "finished";
    QString fileName = m_pluginName + ".zip";   // 压缩文件名
//    file = new QFile(fileName);
//    if (file->exists()) {
//        file->remove();
//    }
//    if (!file->open(QIODevice::WriteOnly |QIODevice::Append)){}
//    file->write(bytes);
//    file->close();
//    emit sig_decompress(fileName);
//    emit over();
    qDebug() << "emit over";
}

void DownloadThread::setProgressBar(qint64 bytesRead, qint64 totalBytes)
{
    emit setProgressValue(bytesRead, totalBytes);
}

void DownloadThread::resetProBar()
{
    emit sigResetProgressBar();
}

// 解压zip文件
void DownloadThread::decompressFile(QString filePath)
{
    if (!QFile(filePath).exists()) {
        qDebug() << filePath << "not exists";
    }
    qDebug() << "压缩文件所在的目录:" << QDir::currentPath();
    QZipReader cZip(filePath);
    foreach(QZipReader::FileInfo item, cZip.fileInfoList())
    {
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
    if (!QDir(m_pluginName).exists()) {
        executeCmd(QString("7z.exe x %1").arg(filePath));
    }
    QFile zipFile(filePath);
    if (zipFile.exists()) {
        zipFile.setPermissions(QFileDevice::WriteGroup);
        bool isDeleted = zipFile.remove();
        if (!isDeleted) {
            QString cmd = "del " + filePath;
            executeCmd(cmd);
        }
    }
    emit afterDecompress();
}



ExistAndMainfest DownloadThread::getOssPackage(QString ossPath, QString pluginName, QString version, QString gitCommit)
{
    QString cdnMetaVerse = "https://meta-download.233xyx.com/";
    QString fullPath = cdnMetaVerse + ossPath;
    QUrl url = fullPath;
    bool isPackageExist = true;
    Manifest mf = {};
    ExistAndMainfest res;
    try {
        qDebug() << "应用程序路径" << QApplication::applicationDirPath();
        qDebug() << QDir::currentPath();
        Download(url);

    }  catch (QException* e) {
        qDebug() << "OssPath error " + ossPath + " " + e->what();
        isPackageExist = false;
        res.isPackageExist = isPackageExist;
        return res;
    }

    qDebug() << "before zipReader" << QDir::currentPath();
    connect(this, &DownloadThread::sig_decompress, this, &DownloadThread::decompressFile);
    connect(this, &DownloadThread::afterDecompress, this, [=](){
        QString newPath = setPath(pluginName);
        QString currentPath = QDir::currentPath();
        if (!newPath.isEmpty()) {
            QDir::setCurrent(newPath);
        }
        else {
            QDir::setCurrent(pluginName);
        }
        QFile version_file("version");
        if (version_file.open(QFile::WriteOnly)) {
            version_file.write(version.toStdString().c_str());
        }
        if (pluginName == "MWTypeScript") {
            executeCmd("npm --registry https://registry.npm.taobao.org install");
        }

    });
    mf = makeManifest(pluginName, false, gitCommit, version);
    res.isPackageExist = isPackageExist;
    res.manifest = mf;

//    QDir::setCurrent(currentPath);
    return res;
}


QString DownloadThread::gitClone(QString gitUrl, QString pluginName, QString branch, QString gitCommit, bool forcePull)
{
    QString cmd = QString("git clone -b %1 %2 %3").arg(branch, gitUrl, pluginName);
    qDebug() << cmd;
    QString res = executeCmd(cmd);
    qDebug() << res;
    QString originalPath = QDir::currentPath();
    QDir::setCurrent(pluginName);
    copyGitHook();
    QDir::setCurrent(originalPath);

    if (!forcePull) {
        cmd = QString("git checkout %1").arg(gitCommit);
        executeCmd(cmd);
    }
    QString commitSha1 = getCommitSha1(pluginName);
    if (pluginName == "MWTypeScript") {
        QString currentPath = QDir::currentPath();
        QDir::setCurrent(pluginName);
        executeCmd("npm --registry https://registry.npm.taobao.org install");
        QDir::setCurrent(currentPath);
    }
    setPath(pluginName);
    return commitSha1;
}


void DownloadThread::getPlugin()
{
    bool forcePull = true;
    QDir dir(QDir::currentPath());
    if (!QDir("Plugins").exists()) {
        dir.mkdir("Plugins");
    }

    QVector<Manifest> commitSha1Manifest;

    bool hasAccess = checkGitAccess(m_gitUrl);

    PluginInfos plugininfo = checkOldVersion(m_pluginName);

    if (plugininfo.hasOldVerison)
    {
        if (plugininfo.isGitRepo)
        {
            if (hasAccess)
            {
                QString currentPath = QDir::currentPath();
                QDir::setCurrent(plugininfo.oldVersionPath);
                QString curBranch = getCurrentBranchName();
                QString targetBranch = branches[m_Env];

                QList<QString> branchesValues = branches.values();
                if (branchesValues.contains(curBranch) || forcePull)
                {
                    executeCmd("git pull");
                    executeCmd("git checkout " + targetBranch);
                    executeCmd("git pull origin " + targetBranch);
                    copyGitHook();
                }
                if (m_pluginName == "MWTypeScript") {
                    executeCmd("npm --registry https://registry.npm.taobao.org install");
                }
                commitSha1Manifest.append(makeManifest(m_pluginName, true, getCommitSha1(), ""));
                QDir::setCurrent(currentPath);
                setNewPath(plugininfo.oldVersionPath, m_pluginName);
                return;
            }
            else
            {
                if (!QDir("Backup").exists()) {
                    QDir("./").mkdir("Backup");
                }
                QString cmd = "robocopy " + plugininfo.oldVersionPath + " Backup/" + m_pluginName + " /e";
                executeCmd(cmd);
                QDir srcDir(plugininfo.oldVersionPath);
                bool isDeleted = srcDir.removeRecursively();    //有可能删除不了，需要改成强制删除
                if (!isDeleted) {
                    qDebug() << "deleted folder " + plugininfo.oldVersionPath + " failed";
                }
                else {
                    qDebug() << "Lost git access of plugin: " + m_pluginName + ", the local git repo has been moved to the /Backup folder";
                }
            }
        }
        else
        {
            if (hasAccess) {
                removeFolder(plugininfo.oldVersionPath);
            }
            else {
                QString currentPath = QDir::currentPath();
                QDir::setCurrent(plugininfo.oldVersionPath);
                commitSha1Manifest.append(makeManifest(m_pluginName, false, m_gitCommit, m_version));
                QString oldVersion = "";
                try {
                    QFile file("verison");
                    if (!file.open(QIODevice::ReadOnly)){
                        qDebug() << "file verison open failed";
                    }
                    oldVersion = file.readAll();
                }  catch (QException* e) {

                }
                if (oldVersion == m_version) {
                    qDebug() << "already up to date";
                    QDir::setCurrent(currentPath);
                    return;
                }
                else {
                    QDir::setCurrent(currentPath);
                    removeFolder(plugininfo.oldVersionPath);
                }
            }
        }
    }
    else {
        qDebug() << "It is a new plugin";
    }
    bool packageExists = false;
    QDir::setCurrent("Plugins");
    if (!hasAccess)
    {
        qDebug() << "execute getOssPackage";
        ExistAndMainfest eam = getOssPackage(m_OSSPath, m_pluginName, m_version, m_gitCommit);
        packageExists = eam.isPackageExist;
        commitSha1Manifest.append(eam.manifest);
    }
    else {
        qDebug() << "execute gitClone";
        QString branch = branches[m_Env];
        QString commitSha1 = gitClone(m_gitUrl, m_pluginName, branch, m_gitCommit, forcePull);
        commitSha1Manifest.append(makeManifest(m_pluginName, true, commitSha1, NULL));
        packageExists = true;
    }

    QDir::setCurrent("../");
    if (!packageExists) {
        qDebug() << "Set package: " + m_pluginName + " failed!";
    }
}


void DownloadThread::run()
{
    m_NetManager = new QNetworkAccessManager;

//    Download(m_url);
    getPlugin();

    exec();
}

