#pragma once

#include <QLocalServer>
#include <QLocalSocket>
#include <QFile>
#include <QEventLoop>
#include <QObject>
#include <QTime>

class Result : public QObject {
  Q_OBJECT

  int m_result;

 public:
  int result() { return m_result; }
  void setResult(int result) {
    m_result = result;
    emit ready();
  }

signals:
  void ready();
};

class Server : public QObject {
  Q_OBJECT

  Result m_result;
  QLocalSocket* m_sock;

 public:
  const int max = 10000;

  void start() {
    QString sockPath = "\\.\pipe\namedpipetest";
    QLocalServer* server = new QLocalServer();
    QFile sockFile(sockPath);
    if (sockFile.exists()) {
      sockFile.remove();
    }
    server->listen(sockPath);
    connect(server, &QLocalServer::newConnection, [this, server]() {
      m_sock = server->nextPendingConnection();
      connect(m_sock, &QLocalSocket::disconnected, m_sock, &QLocalSocket::deleteLater);
      QObject::connect(m_sock, &QLocalSocket::readyRead, this, &Server::onReadyRead,
                       Qt::QueuedConnection);

      QTime startTime = QTime::currentTime();
      for (int i = 0; i < max; i++) {
        sendData(m_sock, 1);
        QEventLoop loop;
        connect(&m_result, &Result::ready, &loop, &QEventLoop::quit);
        //        qDebug("start event loop to wait for 1");
        loop.exec();
        //        qDebug("end event loop to wait for 1");
      }
      int passed = startTime.msecsTo(QTime::currentTime());
      qDebug("total time: %d [ms], count: %d, average: %.2f [ms]", passed, max,
             ((double)passed) / max);
    });
  }

  void onReadyRead() {
    //    qDebug("bytesAvailable: %lld", m_sock->bytesAvailable());
    qint64 bytesAvailable = m_sock->bytesAvailable();
    QByteArray buffer = m_sock->readAll();
    QDataStream ds(buffer);
    while (bytesAvailable > 0) {
      int num;
      ds >> num;
      //      qDebug("received %d", num);
      bytesAvailable -= 4;
      m_result.setResult(num);
    }
  }

  void sendData(QLocalSocket* sock, int num) {
    //    qDebug("send %d", num);
    QByteArray block;
    QDataStream ds(&block, QIODevice::WriteOnly);
    ds << num;
    sock->write(block);
  }
};
