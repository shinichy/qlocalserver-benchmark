#pragma once

#include <QObject>
#include <QLocalSocket>

class Client : public QObject {
  Q_OBJECT

 public:
  QLocalSocket* sock;

  Client() { sock = new QLocalSocket(); }

  void start() {
    connect(sock, &QLocalSocket::readyRead, [this] {
      qint64 bytesAvailable = sock->bytesAvailable();
      QByteArray buffer = sock->readAll();
      QDataStream ds(buffer);
      while (bytesAvailable > 0) {
        int num;
        ds >> num;

        //      qDebug("received %d", num);
        bytesAvailable -= 4;

        sendData(sock, 2);
      }
    });

    sock->connectToServer("\\.\pipe\namedpipetest");
  }

  void sendData(QLocalSocket* sock, int num) {
    //  qDebug("send %d", num);
    QByteArray block;
    QDataStream ds(&block, QIODevice::WriteOnly);
    ds << num;
    sock->write(block);
  }
};
