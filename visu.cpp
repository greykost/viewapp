#include "visu.h"


Program::Program(QWidget *parent) : QMainWindow(parent)
{
    VisuInit(this);

    // Поток
    PixThread = new QThread();
    PixWork = new PixWorker();
    PixWork->moveToThread(PixThread);
    connect(PixThread, &QThread::finished, PixWork, &QObject::deleteLater);

    // Перенаправление поиска
    connect(winCtg, &WinCategory::sigFind, winFnd, &WinSearch::sltFind);

    // При закрытии окон, показать окно категорий
    connect(winObj, &WinObject::sigClose, winCtg, &WinCategory::show);
    connect(winFnd, &WinSearch::sigClose, winCtg, &WinCategory::show);

    // загрузка данных
    manager = new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished, this, &Program::dataPix);
    connect(winCtg, &WinCategory::sigLoadPix, this, &Program::loadPix);
    connect(this, &Program::sigBtnImage, winCtg, &WinCategory::sigBtnImage);

    // Создание секций с категориями
    connect(this, &Program::sigSections, winCtg, &WinCategory::addSections);

    // Клик по категории
    connect(winCtg, &WinCategory::sigLoadObj, winObj, &WinObject::setID);
    connect(this, &Program::sigObjImage, winObj, &WinObject::addObjImage);
    connect(this, &Program::sigSetText, winObj, &WinObject::setText);
    connect(winCtg, &WinCategory::sigLoadObj, PixWork, &PixWorker::getFull);
    connect(PixWork, &PixWorker::sigLoadObj, this, &Program::getFull);
    connect(PixWork, &PixWorker::sigLoadFinish, winObj, &WinObject::pageShow);

    // Запуск потока
    PixThread->start();

    // JSON
    ParseCfg(qApp->applicationDirPath() + QString("/test.json"));
}


void Program::closeEvent(QCloseEvent *e)
{
    QMessageBox::StandardButton rv = QMessageBox::question(0, "Завершение работы программы", "Завершить работу с программой ?");

    //qDebug() << "Program::closeEvent()" << rv;

    if(rv == QMessageBox::Yes)
    {
        PixThread->quit();
        PixThread->wait();
        e->accept();
    }
    else
        return e->ignore();
}


// Открыть JSON-файл
bool Program::OpenCfg(const QString &cfg)
{
    QFile file(cfg);

    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray buf = file.readAll();
    JsonDoc = QJsonDocument::fromJson(buf);
    file.close();

    RootObj = JsonDoc.object();

    return true;
}


// JSON-конфигурация
void Program::ParseCfg(const QString &cfg)
{
    if(OpenCfg(cfg))
    {
        QJsonArray ctg = RootObj.value("categories").toArray();
        for(int i = 0; i < ctg.count(); i++)
        {
            QJsonObject o = ctg[i].toObject();
            QJsonValue id = o["category_id"];
            QJsonValue name = o["name"];
            qDebug() << id.toInt() << name.toString();
        }


        QJsonArray obj = RootObj.value("summary").toArray();
        for(int i = 0; i < obj.count(); i++)
        {
            QJsonObject o = obj.at(i).toObject();
            QJsonValue id = o["object_id"];
            QJsonValue name = o["short_name"];
            QJsonArray ctg = o.value("categories").toArray();
            qDebug() << id.toInt() << name.toString() << ctg;
        }

        QStringList list;
        QJsonArray objf = RootObj.value("full").toArray();
        for(int i = 0; i < objf.count(); i++)
        {
            QJsonObject o = objf.at(i).toObject();
            QJsonValue id = o["object_id"];
            QJsonValue name = o["short_name"];
            QJsonValue content = o["content"];
            QJsonValue title = o["title"];
            QJsonArray ctg = o.value("categories").toArray();
            QJsonArray img = o.value("images").toArray();
            list.append(name.toString());
            list.append(content.toString());
            list.append(title.toString());
            qDebug() << id.toInt() << name.toString() << ctg;
        }

        // Создание секций
        emit sigSections(ctg, obj);

        // Текст для поиска
        winFnd->sltList(list);
    }
}

// Пришел ответ на запрос
void Program::dataPix(QNetworkReply *pReply)
{
    QByteArray data = pReply->readAll();

    int id = pReply->property("id").toInt();

    // Для категории
    if(!pReply->property("trg").toBool())
    {
        QImage img(600, 600, QImage::Format_ARGB32);
        img.loadFromData(data);
        emit sigBtnImage(img , id);
    }

    // В галерею
    if(pReply->property("trg").toBool())
    {
        QImage img(600, 600, QImage::Format_ARGB32);
        img.loadFromData(data);
        emit sigObjImage(img , id);
    }

    pReply->deleteLater();
}

// Отправка запроса
void Program::loadPix(const QUrl &url, int id, int trg)
{
    qDebug() << "Program::loadPix()" << id;
    QNetworkRequest request(url);
    QNetworkReply *pReply = manager->get(request);
    if(pReply)
    {
        if(!pReply->isFinished())
        {
            pReply->setProperty("id", id);
            pReply->setProperty("trg", trg);
        }
        else
            delete pReply;
    }
}

// Данные для галереи
void Program::getFull(int id)
{
    qDebug() << "Program::getFull()" << id;
    //
    QJsonArray objf = RootObj.value("full").toArray();
    for(int i = 0; i < objf.count(); i++)
    {
        QJsonObject o = objf.at(i).toObject();
        QJsonValue oid = o["object_id"];
        if(oid == id)
        {
            QJsonValue name = o["short_name"];
            QJsonValue content = o["content"];
            QJsonValue title = o["title"];
            QJsonArray ctg = o.value("categories").toArray();
            QJsonArray img = o.value("images").toArray();
            //qDebug() << oid.toInt() << name.toString() << ctg;
            for(int j = 0; j < img.count(); j++)
            {
                loadPix(QUrl(img.at(j).toString()), id, 1);
            }
            emit sigSetText(name.toString(), content.toString());
            return;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//==================================== PixWorker ======================================//
/////////////////////////////////////////////////////////////////////////////////////////
PixWorker::PixWorker(QObject *parent) : QObject(parent)
{

}


PixWorker::~PixWorker()
{
    qDebug() << "PixWorker::~PixWorker()";
}


void PixWorker::getFull(int id)
{
    qDebug() << "PixWorker::getFull()" << id;
    emit sigLoadObj(id);
    QThread::sleep(5);
    emit sigLoadFinish();
}
