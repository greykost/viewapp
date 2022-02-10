#ifndef VISU_H
#define VISU_H

#include <QtCore>
#include <QtWidgets>
#include <QtNetwork>
#include "win.h"

class PixWorker;

class VisuWidgets
{
public:
    QWidget         *wdgRoot;
    WinCategory     *winCtg;
    WinSearch       *winFnd;
    WinObject       *winObj;

    void VisuInit(QMainWindow *form)
    {
        form->setObjectName(QString::fromUtf8("forma"));
        form->setWindowTitle("«ViewApp»");
        form->resize(600, 600);

        wdgRoot = new QWidget(form);
        wdgRoot->setObjectName(QString::fromUtf8("wdgRoot"));
        form->setCentralWidget(wdgRoot);

        winCtg = new WinCategory(wdgRoot);
        winCtg->setObjectName(QString::fromUtf8("winCtg"));

        winFnd = new WinSearch(wdgRoot);
        winFnd->setObjectName(QString::fromUtf8("winFnd"));
        winFnd->hide();

        winObj = new WinObject(wdgRoot);
        winObj->setObjectName(QString::fromUtf8("winObj"));
        winObj->hide();

        QGridLayout *grid = new QGridLayout;
        grid->setContentsMargins(8, 8, 8, 8);
        grid->addWidget(winCtg, 0, 0, 1, 1);
        grid->addWidget(winFnd, 0, 0, 1, 1);
        grid->addWidget(winObj, 0, 0, 1, 1);
        wdgRoot->setLayout(grid);
    }
};


//
class Program : public QMainWindow, private VisuWidgets
{
    Q_OBJECT
public:
    explicit Program(QWidget *parent = 0);
    
signals:
    void sigSections(const QJsonArray &sec, const QJsonArray &obj);
    void sigObjImage(const QImage &img, int id);
    void sigBtnImage(const QImage &img, int id);
    void sigSetText(const QString &name, const QString &txt);

public slots:
    void loadPix(const QUrl &url, int id, int trg);

private slots:
    void dataPix(QNetworkReply *pReply);
    void getFull(int id);
    bool OpenCfg(const QString &cfg);
    void ParseCfg(const QString &cfg);

private:
    QJsonDocument           JsonDoc;
    QJsonObject             RootObj;
    QThread                 *PixThread;
    PixWorker               *PixWork;
    QNetworkAccessManager   *manager;

protected:
    void closeEvent(QCloseEvent *e);
};


class PixWorker : public QObject
{
    Q_OBJECT

public:
    explicit PixWorker(QObject *parent = nullptr);

    virtual ~PixWorker();

signals:
    void sigLoadObj(int id);
    void sigLoadFinish();

public slots:
    void getFull(int id);
};

#endif // VISU_H
