#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QCheckBox>
#include <QToolButton>
#include <QProgressBar>
#include <QDebug>

#include "networkoperate.h"
#include "downloadprepare.h"
#include "downloadthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void on_FullSelectBtn_clicked();
    void CreateView(int rows, int columns);

private slots:
    void on_DownloadBtn_clicked();

private:
    Ui::MainWindow *ui;

    bool isChecked;
    QStandardItemModel *m_model;
    QTableView *tableView;
    QSharedPointer<NetworkOperate>  np;

    QSharedPointer<DownLoadPlugins> m_download_ptr;
    QMap<QString, QJsonObject> resources;
};
#endif // MAINWINDOW_H
