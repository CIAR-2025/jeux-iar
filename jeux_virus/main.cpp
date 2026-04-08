#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "virus_controller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    VirusController gameController;
    engine.rootContext()->setContextProperty("gameController", &gameController);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("jeux_virus", "Main");

    return QCoreApplication::exec();
}
