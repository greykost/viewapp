#include <visu.h>
#include <QApplication>
#include <QTextCodec>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    Program window;
    window.show();

    return app.exec();
}

