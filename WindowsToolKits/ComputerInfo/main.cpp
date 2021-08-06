#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "hardware/HardwareInfo.h"
#include "ComputerInfo.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("ComputerInfo",new ComputerInfo);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
