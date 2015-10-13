#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include "server.h"

int main(int argv, char** args) {
  QCoreApplication app(argv, args);

  Server server;
  server.start();

  return app.exec();
}
