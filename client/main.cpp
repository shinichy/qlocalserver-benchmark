#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include "client.h"

int main(int argv, char** args) {
  QCoreApplication app(argv, args);

  Client client;
  client.start();

  return app.exec();
}
