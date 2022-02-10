#include "win.h"


/////////////////////////////////////////////////////////////
Catbutton::Catbutton(const QString &name, int id, QWidget *parent) : ScaledImage(name, parent), oname(name), oid(id)
{
    setEnabled(true);
    setCursor(QCursor(Qt::PointingHandCursor));
    setMinimumSize(150, 75);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QSizePolicy sp = sizePolicy();
    sp.setVerticalStretch(1);
    setSizePolicy(sp);
}


void Catbutton::setImage(const QImage &img, int id)
{
    if(oid == id && !img.isNull())
    {
        imageUpdate(img);
    }
}


void Catbutton::mousePressEvent(QMouseEvent *e)
{
    emit sigLoadObj(oid);
    e->accept();
}


///////////////////////////////////////////////////////////////////
CatSection::CatSection(const QString &name, int id, const QJsonArray &obj, QWidget *parent) : QWidget(parent), secname(name), secid(id), ctgobj(obj)
{
    VisuInit();
}


void CatSection::addCategories()
{
    int count = 0;
    for(int i = 0; i < ctgobj.count(); i++)
    {
        QJsonObject o = ctgobj.at(i).toObject();
        QJsonValue oid = o["object_id"];
        QJsonValue name = o["short_name"];
        QJsonValue img = o["image"];
        QJsonArray ctg = o.value("categories").toArray();
        //qDebug() << oid.toInt() << name.toString() << ctg;

        for(int j = 0; j < ctg.count(); j++)
        {
            if(ctg.at(j) == secid)
            {
                count++;
                Catbutton *btn = new Catbutton(name.toString(), oid.toInt(), this);
                grid->addWidget(btn, count / 4, count % 3, 1, 1);
                connect(btn, &Catbutton::sigLoadPix, this, &CatSection::sigLoadPix);
                connect(this, &CatSection::sigBtnImage, btn, &Catbutton::setImage);
                connect(btn, &Catbutton::sigLoadObj, this, &CatSection::sigLoadObj);
                // запрос картинки
                QUrl imageUrl(img.toString());
                emit btn->sigLoadPix(imageUrl, oid.toInt(), 0);
            }
        }
    }
    QSizePolicy sp = sizePolicy();
    sp.setVerticalStretch((count / 4) + 1);
    setSizePolicy(sp);
    vl->addLayout(grid);
    setLayout(vl);
}



/////////////////////////////////////////////////////////////////////////////////////////
WinCategory::WinCategory(QWidget *parent) : QWidget(parent)
{
    VisuInit();
    connect(btnFind, &QPushButton::clicked, this, &WinCategory::finded);
}


void WinCategory::addSections(const QJsonArray &sec, const QJsonArray &obj)
{
    for(int i = 0; i < sec.count(); i++)
    {
        QJsonObject o = sec.at(i).toObject();
        QJsonValue id = o["category_id"];
        QJsonValue name = o["name"];
        //qDebug() << id.toInt() << name.toString();
        bool empty = true;
        for(int j = 0; j < obj.count(); j++)
        {
            QJsonObject o = obj.at(j).toObject();
            QJsonArray ctg = o.value("categories").toArray();
            for(int c = 0; c < ctg.count(); c++)
            {
                if(ctg.at(c).toInt() == id.toInt())
                {
                    empty = false;
                    break;
                }
            }
        }
        if(empty)
            continue;

        CatSection *ctgsec = new CatSection(name.toString(), id.toInt(), obj, this);
        connect(ctgsec, &CatSection::sigLoadPix, this, &WinCategory::sigLoadPix);
        connect(this, &WinCategory::sigBtnImage, ctgsec, &CatSection::sigBtnImage);
        connect(ctgsec, &CatSection::sigLoadObj, this, &WinCategory::sigLoadObj);
        connect(ctgsec, &CatSection::sigLoadObj, this, &WinCategory::hide);
        ctgsec->addCategories();
        vl->addWidget(ctgsec);
    }
    vl->addStretch();
    //setLayout(vl);
}


void WinCategory::finded()
{
    emit sigFind(editFind->text());
    hide();
}




////////////////////////////////////////////////////////////////////////////////
WinSearch::WinSearch(QWidget *parent) : QWidget(parent)
{
    VisuInit();

    connect(btnFind, &QPushButton::clicked, this, &WinSearch::finded);
    connect(btnClose, &QPushButton::clicked, this, &WinSearch::sigClose);
    connect(btnClose, &QPushButton::clicked, this, &WinSearch::hide);
}


void WinSearch::finded()
{
    qDebug() << "WinSearch::find()";
    if(!editFind->text().isEmpty())
    {
        listPoisk->clear();
        for(int i = 0; i < list.count(); i++)
        {
            if(list.at(i).indexOf(editFind->text()) != -1)
            {
                QListWidgetItem *elm = new QListWidgetItem(list.at(i), listPoisk);
                listPoisk->addItem(elm);
            }
        }
    }
}


void WinSearch::sltFind(const QString &txt)
{
    show();
    editFind->setText(txt);
    btnFind->click();
}




////////////////////////////////////////////////////////////////////////
WinObject::WinObject(QWidget *parent) : QWidget(parent), curobject(0), bufpos(-1), bufimg(0), timer(this), progress(0), oname(""), otext("")
{
    VisuInit();
    connect(btnClose, &QPushButton::clicked, this, &WinObject::sigClose);
    connect(btnClose, &QPushButton::clicked, this, &WinObject::hide);
    connect(btnNext, &QPushButton::clicked, this, &WinObject::next);
    connect(btnPrev, &QPushButton::clicked, this, &WinObject::prev);
    connect(&timer, &QTimer::timeout, this, &WinObject::load);
    connect(this, &WinObject::sigImageShow, obj, &ScaledImage::imageUpdate);
}


// Если брать сохраненное в кнопке, а не грузить каждый раз.
/*void WinObject::setData(const QString &name, const QString &txt, const QVector<QImage> &buf)
{
    lblText->setText("<html>" + txt + "</html>");
    lblName->setText("<html>" + name + "</html>");
    bufimg = buf;
}*/


void WinObject::setText(const QString &name, const QString &txt)
{
    oname = "<html>" + name + "</html>";
    otext = "<html>" + txt + "</html>";
    lblName->setText(oname);
}


void WinObject::pageShow()
{
    pbar->hide();
    timer.stop();
    progress = 0;

    lblText->setText(otext);

    if(bufimg.count() >= 1)
    {
        bufpos = 0;
        emit sigImageShow(bufimg.at(bufpos));
    }

    if(bufimg.count() > 1)
        btnNext->setEnabled(true);
}


void WinObject::next()
{
    if(!bufimg.isEmpty())
    {
        bufpos++;
        if(bufpos >= (bufimg.count() - 1))
        {
            bufpos = bufimg.count() - 1;
            btnNext->setEnabled(false);
        }
        btnPrev->setEnabled(true);
        qDebug() << "WinObject::next()" << bufpos;
        emit sigImageShow(bufimg.at(bufpos));
    }
}


void WinObject::prev()
{
    if(!bufimg.isEmpty())
    {
        bufpos--;
        if(bufpos <= 0)
        {
            bufpos = 0;
            btnPrev->setEnabled(false);
        }
        btnNext->setEnabled(true);
        qDebug() << "WinObject::prev()" << bufpos;
        emit sigImageShow(bufimg.at(bufpos));
    }
}


void WinObject::load()
{
   progress++;
   pbar->setValue(progress);
}


void WinObject::setID(int id)
{
    qDebug() << "WinObject::setID()" << id;
    curobject = id;
    timer.start(100);
    show();
    pbar->setGeometry(0, (height()/2) - 50, width(), 100);
    pbar->show();
    lblText->setText("");
    lblName->setText("");
    obj->imageClear();
    btnPrev->setEnabled(false);
    btnNext->setEnabled(false);

    bufimg.clear();
}


void WinObject::addObjImage(const QImage &img, int id)
{
    //qDebug() << "WinObject::addObjImage()" << id;
    if(curobject == id)
    {
        bufimg.push_back(img);
    }
}




///////////////////////////////////////////////////////////
ScaledImage::ScaledImage(QString text, QWidget *parent) : QWidget(parent), ptext(text)
{
    VisuInit();

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QSizePolicy sp = sizePolicy();
    sp.setVerticalStretch(1);
    setSizePolicy(sp);
}


void ScaledImage::imageClear()
{
    QPalette palette(this->palette());
    loadImage.fill(palette.color(QPalette::Background));
}


void ScaledImage::imageUpdate(const QImage &img)
{
    loadImage = img;
    update();
}


void ScaledImage::paintEvent(QPaintEvent *event)
{
    if(loadImage.isNull())
        return;

    QPainter painter(this);
    QImage buf(size(), QImage::Format_ARGB32);

    if((loadImage.width() > width() || loadImage.height() > height()) || (loadImage.width() < width() || loadImage.height() < height()))
        buf = loadImage.width() > loadImage.height()  ? loadImage.scaledToWidth(width()) : loadImage.scaledToHeight(height());
    else buf = loadImage.copy();

    painter.drawImage(0, 0, buf);
    event->accept();
}

