#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isChecked = false;
    m_model = new QStandardItemModel();
    tableView = ui->PluginsManager;

    ui->tabWidget->setTabText(0, "插件管理");
    ui->tabWidget->setTabText(1, "插件注册");

    np = NetworkOperate::Create();

    m_download_ptr = QSharedPointer<DownLoadPlugins>(new DownLoadPlugins);
//    resources = m_download_ptr->getDependPluginList();


    // 当网络资源请求完毕之后，再触发CreateView函数
//    connect(np.get(), SIGNAL(finished(int,int)), this, SLOT(CreateView(int,int)));
    connect(np.get(), &NetworkOperate::finished, m_download_ptr.get(), &DownLoadPlugins::getDependPluginList);
    connect(m_download_ptr.get(), &DownLoadPlugins::signalCreateTable, this, &MainWindow::CreateView);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_model;
}

void MainWindow::CreateView(int rows, int columns)
{
    QStringList horizontalHeader;
    horizontalHeader << "选择" << "插件名称" << "最新版本" << "本地版本/分支/有无权限" << "下载进度";
    if (!m_model) {
        m_model = new QStandardItemModel();
    }
    m_model->setHorizontalHeaderLabels(horizontalHeader);
    m_model->setColumnCount(columns);
    m_model->setRowCount(rows);
    tableView->verticalHeader()->hide();
    tableView->setModel(m_model);
    //表头qss设置
    QString strHeaderQss;
    strHeaderQss = "QHeaderView::section { background:white; color:black;min-height:3em;}";
    tableView->setStyleSheet(strHeaderQss);

//    QList<QJsonObject> resources_arr = m_download_ptr->getProResouceArr();

    tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);        //表头信息显示居中
    tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);  //设定表头列宽不可变
    tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch); //设定第五列表头弹性拉伸
    tableView->setColumnWidth(0, 50);       //设定表格第0列宽度
    tableView->setColumnWidth(1, 150);
    tableView->setColumnWidth(2, 160);
    tableView->setColumnWidth(3, 350);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选中时整行选中
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置表格属性只读，不能编辑

    for(int i = 0; i < rows; i++)
    {
        QStandardItem *standItem0 = new QStandardItem(tr("%1").arg(i+1));
        QStandardItem *standItem1 = new QStandardItem(tr("%1").arg(m_download_ptr->getPluginName(i)));
        QStandardItem *standItem2 = new QStandardItem(tr("%1").arg(m_download_ptr->getVersion(i)));
//        QStandardItem *standItem4 = ne
        m_model->setItem(i, 0, standItem0);
        m_model->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        m_model->setItem(i,1,standItem1);                                //表格第i行，第0列添加一项内容
        // m_model->item(i, 1)->setForeground(QBrush(QColor(255, 0, 0)));      //设置字符颜色
        m_model->item(i, 1)->setTextAlignment(Qt::AlignCenter);           //设置表格内容居中
        m_model->setItem(i, 2, standItem2);                                //表格第i行，第1列添加一项内容
        m_model->item(i, 2)->setTextAlignment(Qt::AlignCenter);
    }
    for (int i = 0; i < rows; i++)
    {
        m_model->item(i, 0)->setCheckable(true);
    }
//    tableView->setIndexWidget(m_model->index(0, 4), new QProgressBar());
}

void MainWindow::on_FullSelectBtn_clicked()
{
    int rows = tableView->model()->rowCount();
    if (isChecked) {
        for (int i = 0; i < rows; i++) {
            m_model->item(i, 0)->setCheckState(Qt::Unchecked);
        }
        isChecked = false;
        ui->FullSelectBtn->setText("全选");
    }
    else {
        for (int i = 0; i < rows; i++) {
            m_model->item(i, 0)->setCheckState(Qt::Checked);
        }
        isChecked = true;
        ui->FullSelectBtn->setText("取消");
    }
}


void MainWindow::on_DownloadBtn_clicked()
{
//    m_download_ptr = QSharedPointer<DownLoadPlugins>(new DownLoadPlugins);
//    m_download_ptr->getDependPluginList();

//    DownloadThread* p = new DownloadThread();
//    p->start();
//     qDebug() << QDir::currentPath();
//    QString absFileName = "D:/gitcloneProjects/metaworldeditor/Plugins/MWEd/MWEdCommon/path_config.ini";
//    QSharedPointer<QSettings> configIniRead = QSharedPointer<QSettings>(new QSettings(absFileName, QSettings::IniFormat));
//    QString moduleName = configIniRead->value("/mainModule/mainModuleName").toString();
//    qDebug() << moduleName;

//    QString pathName = "MWEdCommon";
//    QString newPath = "Plugins/MWEdCommon";
//    QDir srcDir(pathName);
//    bool msg = srcDir.removeRecursively();

//    qDebug() << msg;
    const QString url = "https://meta-download.233xyx.com/Plugins/275293132/3d6c8dc3cf2bfb7e87ecb40be8f92b99ca9dd658";
//    const QString url = "http://test-plugins.meta-verse.co/Plugin/List";

//    QUrl linkUrl(url);
//    DownloadThread* dt = new DownloadThread(linkUrl);
//    auto pb = new QProgressBar(tableView);
//    QString style = "QProgressBar{border: 2px solid grey;  border-radius: 5px;   background-color: #FFFFFF; text-align: center;}QProgressBar::chunk {background-color: green; width: 2px;}";
//    pb->setStyleSheet(style);

//    tableView->setIndexWidget(m_model->index(0, 4), pb);
//    dt->start();
//    connect(dt, &DownloadThread::setProgressValue, pb, [=](qint64 bytesRead, qint64 totalBytes){
//        pb->setMaximum(totalBytes);
//        pb->setValue(bytesRead);
//    });
//    connect(dt, &DownloadThread::sigResetProgressBar, pb, [=](){
//        pb->setValue(0);
//    });

    int rows = m_download_ptr->getProResouceArr().size();
    QMap<QString, QJsonObject> resources = m_download_ptr->getResources();

    for (int i = 0; i < rows; i++) {
        bool ischecked = m_model->item(i, 0)->checkState();
        if (ischecked) {
            qDebug() << i + 1 << m_model->item(i, 1)->data(Qt::DisplayRole).toString();
            QString key = m_model->item(i, 1)->data(Qt::DisplayRole).toString();
            QJsonObject obj = resources.value(key);
            QString pluginName = obj.value("name").toString();
            QString gitUrl = obj.value("gitUrl").toString();
            QString ossPath = obj.value("OSSPath").toString();
            QString gitCommit = obj.value("gitCommit").toString();
            QString verison = obj.value("version").toString();
            QString env = m_download_ptr->getEnviroment();
            qint64 file_size = obj.value("fileSize").toDouble();
            qDebug() << "文件大小：" << file_size;
            auto dt = new DownloadThread(pluginName, gitUrl, ossPath, gitCommit, verison, env, file_size);
            auto pb = new QProgressBar(tableView);
            pb->setRange(0, 10000);
            QString style = "QProgressBar{border: 2px solid grey;  border-radius: 5px;   background-color: #FFFFFF; text-align: center;}QProgressBar::chunk {background-color: green; width: 2px;}";
            pb->setStyleSheet(style);

            tableView->setIndexWidget(m_model->index(i, 4), pb);
            dt->start();
            connect(dt, &DownloadThread::setProgressValue, pb, [=](qint64 bytesRead, qint64 totalBytes){
//                pb->setMaximum(10000);
                pb->setValue((bytesRead*10000)/totalBytes);
            });
            connect(dt, &DownloadThread::sigResetProgressBar, pb, [=](){
                pb->setValue(0);
//                ui->DownloadBtn->setEnabled(true);
            });
            connect(dt, &DownloadThread::over, this, [&](){
//                dt->quit();
//                dt->wait();
//                dt->deleteLater();
//                dt = nullptr;
            });
        }
    }
    ui->DownloadBtn->setDisabled(true);
}

