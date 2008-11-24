/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#include "localsocketclient.h"
#include "clientconnection.h"
#include "clientmodel.h"

#include <QtCore/QDir>
#include <QtCore/QThread>
#include <QtNetwork/QLocalSocket>

Q_DECLARE_METATYPE( QLocalSocket::LocalSocketError )
Q_DECLARE_METATYPE( QAbstractSocket::SocketError )
Q_DECLARE_METATYPE( QAbstractSocket::SocketState )

namespace Soprano {
    namespace Client {
        class LocalSocketClientConnection : public ClientConnection
        {
        public:
            LocalSocketClientConnection( const QString& path, QObject* parent );
            ~LocalSocketClientConnection();

        protected:
            QIODevice* newConnection();
            bool isConnected( QIODevice* );

        private:
            QString m_socketPath;
        };

        LocalSocketClientConnection::LocalSocketClientConnection( const QString& path, QObject* parent )
            : ClientConnection( parent ),
              m_socketPath( path )
        {
        }

        LocalSocketClientConnection::~LocalSocketClientConnection()
        {
        }

        QIODevice* LocalSocketClientConnection::newConnection()
        {
            clearError();
            QString path( m_socketPath );
            if ( path.isEmpty() ) {
                path = QDir::homePath() + QLatin1String( "/.soprano/socket" );
            }

            QLocalSocket* socket = new QLocalSocket;
            socket->connectToServer( path, QIODevice::ReadWrite );
            if ( socket->waitForConnected() ) {
                QObject::connect( socket, SIGNAL( error( QLocalSocket::LocalSocketError ) ),
                                  parent(), SLOT( _s_localSocketError( QLocalSocket::LocalSocketError ) ) );
                return socket;
            }
            else {
                setError( socket->errorString() );
                delete socket;
                return 0;
            }
        }

        bool LocalSocketClientConnection::isConnected( QIODevice* device ) {
            return( static_cast<QLocalSocket*>( device )->state() == QLocalSocket::ConnectedState );
        }
    }
}


class Soprano::Client::LocalSocketClient::Private
{
public:
    Private()
        : connection( 0 ) {
    }

    LocalSocketClientConnection* connection;

    void _s_localSocketError( QLocalSocket::LocalSocketError );
};


void Soprano::Client::LocalSocketClient::Private::_s_localSocketError( QLocalSocket::LocalSocketError error )
{
    qDebug() << "local socket error:" << error;
}



Soprano::Client::LocalSocketClient::LocalSocketClient( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    qRegisterMetaType<QLocalSocket::LocalSocketError>();
    qRegisterMetaType<QAbstractSocket::SocketError>();
    qRegisterMetaType<QAbstractSocket::SocketState>();
}


Soprano::Client::LocalSocketClient::~LocalSocketClient()
{
    disconnect();
    delete d;
}


bool Soprano::Client::LocalSocketClient::connect( const QString& name )
{
    if ( !d->connection ) {
        d->connection = new LocalSocketClientConnection( name, this );
        if ( d->connection->testConnection() &&
             d->connection->checkProtocolVersion() ) {
            return true;
        }
        else {
            disconnect();
            return false;
        }
    }
    else {
        setError( "Already connected" );
        return false;
    }
}


bool Soprano::Client::LocalSocketClient::isConnected() const
{
    return d->connection ? d->connection->testConnection() : false;
}


void Soprano::Client::LocalSocketClient::disconnect()
{
    delete d->connection;
    d->connection = 0;
}


Soprano::Model* Soprano::Client::LocalSocketClient::createModel( const QString& name, const QList<BackendSetting>& settings )
{
    if ( d->connection ) {
        int modelId = d->connection->createModel( name, settings );
        setError( d->connection->lastError() );
        if ( modelId > 0 ) {
            StorageModel* model = new ClientModel( 0, modelId, d->connection );
            return model;
        }
    }
    else {
        setError( "Not connected" );
    }

    return 0;
}


void Soprano::Client::LocalSocketClient::removeModel( const QString& name )
{
    if ( d->connection ) {
        d->connection->removeModel( name );
        setError( d->connection->lastError() );
    }
    else {
        setError( "Not connected" );
    }
}

#include "localsocketclient.moc"
