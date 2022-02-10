#ifndef WIN_H
#define WIN_H

#include <QtWidgets>
#include <QtNetwork>

class ScaledImage : public QWidget
{
    Q_OBJECT

public:
    explicit ScaledImage(QString text = QString(), QWidget *parent = nullptr);

    virtual ~ScaledImage() {  }

public slots:
    //void setName(const QString &name) { lblName->setText(name); }
    void imageUpdate(const QImage &img);
    void imageClear();

public:

private:
    QImage      loadImage;
    QString     ptext;
    QLabel      *lblName;

    void VisuInit()
    {
        setObjectName("ScaledImage");

        lblName = new QLabel(this);
        lblName->setObjectName("lblName");
        lblName->setWordWrap(true);
        lblName->setStyleSheet("color: white; font: bold; font-size: 10px;");
        lblName->setText(ptext);

        QVBoxLayout *vl = new QVBoxLayout;
        vl->addWidget(lblName);
        vl->addStretch();
        setLayout(vl);
    }

protected:
    void paintEvent(QPaintEvent *event);
};



class Catbutton : public ScaledImage
{
    Q_OBJECT

public:
    explicit Catbutton(const QString &name, int id, QWidget *parent = nullptr);

    virtual ~Catbutton() {  }

signals:
    void sigLoadPix(const QUrl &url, int id, int trg);
    void sigLoadObj(int id);

private:

public slots:
    void setImage(const QImage &img, int id);

private:
    QString         oname;
    int             oid;

protected:
    void mousePressEvent(QMouseEvent *e);
};



class CatSection : public QWidget
{
    Q_OBJECT

public:
    explicit CatSection(const QString &name, int id, const QJsonArray &obj, QWidget *parent = nullptr);

    virtual ~CatSection() {  }

signals:
    void sigLoadPix(const QUrl &url, int id, int trg);
    void sigLoadObj(int id);
    void sigBtnImage(const QImage &img, int id);

public slots:
    void addCategories();

private:
    QString         secname;
    int             secid;
    QJsonArray      ctgobj;

private:
    QLabel          *lblName;
    QGridLayout     *grid;
    QVBoxLayout     *vl;

    void VisuInit()
    {
        setObjectName("CatSection");

        lblName = new QLabel(this);
        lblName->setObjectName("lblName");
        lblName->setText(secname);

        grid = new QGridLayout;
        grid->setContentsMargins(0, 8, 0, 8);

        vl = new QVBoxLayout;
        vl->addWidget(lblName);
        //vl->addLayout(grid);
        //setLayout(vl);

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
};



class WinCategory : public QWidget
{
    Q_OBJECT

public:
    explicit WinCategory(QWidget *parent = nullptr);

    virtual ~WinCategory() {  }

signals:
    void sigFind(const QString &txt);
    void sigLoadPix(const QUrl &url, int id, int trg);
    void sigLoadObj(int id);
    void sigBtnImage(const QImage &img, int id);

public slots:
    void addSections(const QJsonArray &sec, const QJsonArray &obj);

private slots:
    void finded();

private:
    QLineEdit       *editFind;
    QPushButton     *btnFind;
    QVBoxLayout     *vl;

    void VisuInit()
    {
        setObjectName("WinCategory");

        editFind = new QLineEdit(this);
        editFind->setObjectName(QString::fromUtf8("editFind"));
        editFind->setPlaceholderText(tr("Введите слова для поиска"));

        btnFind = new QPushButton(this);
        btnFind->setObjectName("btnFind");
        btnFind->setMaximumSize(60, 30);
        btnFind->setText(tr("Найти"));

        QHBoxLayout *hl = new QHBoxLayout;
        hl->setContentsMargins(8, 2, 8, 2);
        hl->addWidget(editFind);
        hl->addWidget(btnFind);

        vl = new QVBoxLayout;
        vl->setMargin(0);
        vl->addLayout(hl);

        setLayout(vl);

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
};



class WinSearch : public QWidget
{
    Q_OBJECT

public:
    explicit WinSearch(QWidget *parent = nullptr);

    virtual ~WinSearch() {  }

signals:
    void sigClose();

public slots:
    void sltFind(const QString &txt);
    void sltList(const QStringList &lst) { list = lst; }

private slots:
    void finded();

private:
    QStringList     list;

private:
    QLineEdit       *editFind;
    QLabel          *lblName;
    QPushButton     *btnFind;
    QPushButton     *btnClose;
    QListWidget     *listPoisk;

    void VisuInit()
    {
        setObjectName("WinSearch");

        btnClose = new QPushButton(this);
        btnClose->setObjectName("btnClose");
        btnClose->setMaximumSize(60, 30);
        btnClose->setText(tr("<< "));

        editFind = new QLineEdit(this);
        editFind->setObjectName(QString::fromUtf8("editFind"));
        editFind->setPlaceholderText(tr("Введите слова для поиска"));

        lblName = new QLabel(this);
        lblName->setObjectName("lblName");
        lblName->setText(tr("Результат поиска:"));

        btnFind = new QPushButton(this);
        btnFind->setObjectName("btnFind");
        btnFind->setMaximumSize(60, 30);
        btnFind->setText(tr("Найти"));

        listPoisk = new QListWidget(this);
        listPoisk->setObjectName("listPoisk");

        QHBoxLayout *hl0 = new QHBoxLayout;
        hl0->addWidget(btnClose);
        hl0->addStretch();

        QHBoxLayout *hl = new QHBoxLayout;
        hl->addWidget(editFind);
        hl->addWidget(btnFind);

        QVBoxLayout *vl = new QVBoxLayout;
        vl->addLayout(hl0);
        vl->addLayout(hl);
        vl->addWidget(lblName);
        vl->addWidget(listPoisk);
        //vl->addStretch();

        setLayout(vl);
    }
};



class WinObject : public QWidget
{
    Q_OBJECT

public:
    explicit WinObject(QWidget *parent = nullptr);

    virtual ~WinObject() {  }

signals:
    void sigClose();
    void sigImageShow(const QImage &img);

public slots:
    void setID(int id);
    //void setData(const QString &name, const QString &txt, const QVector <QImage> &buf);
    void addObjImage(const QImage &img, int id);
    void setText(const QString &name, const QString &txt);
    void pageShow();

private slots:
    void next();
    void prev();
    void load();

private:
    int                 curobject;
    int                 bufpos;
    QVector <QImage>    bufimg;
    QTimer              timer;
    int                 progress;
    QString             oname, otext;

private:
    QLabel          *lblName;
    QLabel          *lblText;
    //QLabel          *lblPix;
    QPushButton     *btnPrev;
    QPushButton     *btnNext;
    QPushButton     *btnClose;
    QProgressBar    *pbar;
    ScaledImage      *obj;

    void VisuInit()
    {
        setObjectName("WinObject");
        //QGridLayout *grid = new QGridLayout;
        //grid->setContentsMargins(8, 8, 8, 8);

        btnClose = new QPushButton(this);
        btnClose->setObjectName("btnNext");
        btnClose->setMaximumSize(60, 30);
        btnClose->setText(tr("<< "));

        lblName = new QLabel(this);
        lblName->setObjectName("lblName");
        lblName->setText(tr("Заголовок"));

        lblText = new QLabel(this);
        lblText->setObjectName("lblText");
        lblText->setText(tr("Текст"));
        lblText->setWordWrap(true);

        obj = new ScaledImage("", this);

        btnPrev = new QPushButton(this);
        btnPrev->setObjectName("btnPrev");
        btnPrev->setMaximumSize(60, 30);
        btnPrev->setText(tr("<<"));

        btnNext = new QPushButton(this);
        btnNext->setObjectName("btnNext");
        btnNext->setMaximumSize(60, 30);
        btnNext->setText(tr(">>"));

        pbar = new QProgressBar(this);
        pbar->setOrientation(Qt::Horizontal);
        pbar->setRange(0, 50);
        pbar->setFormat("Загрузка данных: %p%");

        QHBoxLayout *hl = new QHBoxLayout;
        hl->addWidget(btnClose);
        hl->addStretch();

        QHBoxLayout *hl1 = new QHBoxLayout;
        hl1->addStretch();
        hl1->addWidget(btnPrev);
        hl1->addWidget(btnNext);

        QVBoxLayout *vl = new QVBoxLayout;
        vl->addLayout(hl);
        vl->addWidget(lblName);
        vl->addWidget(lblText);
        vl->addWidget(obj);
        vl->addLayout(hl1);
        vl->addStretch();

        //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setLayout(vl);
    }
};

#endif // WIN_H
