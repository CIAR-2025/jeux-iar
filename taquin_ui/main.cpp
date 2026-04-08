#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "taquin_game.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    TaquinGame game;
    engine.rootContext()->setContextProperty("taquinGame", &game);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("taquin_ui", "Main");

    return QCoreApplication::exec();
}
