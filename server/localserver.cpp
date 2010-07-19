/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "localserver.h"
#include "serverconnection.h"
#include "servercore_p.h"

#include <QtCore/QDebug>
#include <QtNetwork/QLocalSocket>

namespace {
    class LocalServerConnection : public Soprano::Server::ServerConnection
    {
    public:
        LocalServerConnection( quintptr socketDescriptor, Soprano::Server::ModelPool* pool, Soprano::Server::ServerCore* core )
            : ServerConnection( pool, core ) {
            m_socketDescriptor = socketDescriptor;
        }

    protected:
        QIODevice* createIODevice() {
            QLocalSocket* socket = new QLocalSocket();
            socket->setSocketDescriptor( m_socketDescriptor );
            return socket;
        }

    private:
        quintptr m_socketDescriptor;
    };
}


Soprano::Server::LocalServer::LocalServer( ServerCorePrivate* serverCore, QObject* parent )
    : QLocalServer( parent ),
      m_serverCore( serverCore )
{
}


Soprano::Server::LocalServer::~LocalServer()
{
}


void Soprano::Server::LocalServer::incomingConnection( quintptr socketDescriptor )
{
    qDebug() << Q_FUNC_INFO;
    if ( m_serverCore->maxConnectionCount > 0 &&
         m_serverCore->connections.count() >= m_serverCore->maxConnectionCount ) {
        qDebug() << Q_FUNC_INFO << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA too many conenctions! go away!";
    }
    else {
        LocalServerConnection* conn = new LocalServerConnection( socketDescriptor, m_serverCore->modelPool, m_serverCore->q );
        m_serverCore->addConnection( conn );
    }
}

#include "localserver.moc"
