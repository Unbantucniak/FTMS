#include "tcp_server.h"
#include <QDebug>
#include "client_handler.h"

TcpServer::TcpServer(QObject* parent)
	: QTcpServer(parent) {}

void TcpServer::incomingConnection(qintptr socketDescriptor) {
	qDebug() << "新的客户端连接，描述符：" << socketDescriptor;
	auto* handler = new ClientHandler(socketDescriptor, this);
	connect(handler, &ClientHandler::finished, handler, &ClientHandler::deleteLater);
	handler->start();
}
