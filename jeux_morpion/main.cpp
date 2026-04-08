#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "morpion_controller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    MorpionController controller;
    engine.rootContext()->setContextProperty("morpionController", &controller);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("jeux_morpion", "Main");

    return QCoreApplication::exec();
}
