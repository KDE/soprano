/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#include "tcpclient.h"
#include "clientconnection.h"
#include "clientmodel.h"

#include <QtNetwork/QTcpSocket>
#include <QDebug>


Q_DECLARE_METATYPE( QAbstractSocket::SocketError )


const quint16 Soprano::Client::TcpClient::DEFAULT_PORT = 5000;


namespace Soprano {
    namespace Client {
        class TcpClientConnection : public ClientConnection
        {
        public:
            TcpClientConnection( const QHostAddress& address, int port, QObject* parent );
            ~TcpClientConnection();

        protected:
            QIODevice* newConnection();
            bool isConnected( QIODevice* );

        private:
            QHostAddress m_address;
            int m_port;
        };

        TcpClientConnection::TcpClientConnection( const QHostAddress& address, int port, QObject* parent )
            : ClientConnection( parent ),
              m_address( address ),
              m_port( port )
        {
        }

        TcpClientConnection::~TcpClientConnection()
        {
        }

        QIODevice* TcpClientConnection::newConnection()
        {
            clearError();

            QTcpSocket* socket = new QTcpSocket;
            socket->connectToHost( m_address, m_port );
            if ( socket->waitForConnected() ) {
                QObject::connect( socket, SIGNAL(error(QAbstractSocket::SocketError)),
                                  parent(), SLOT(slotError(QAbstractSocket::SocketError)) );
                return socket;
            }
            else {
                setError( socket->errorString() );
                delete socket;
                return 0;
            }
        }

        bool TcpClientConnection::isConnected( QIODevice* device ) {
            return( static_cast<QTcpSocket*>( device )->state() == QAbstractSocket::ConnectedState );
        }
    }
}


class Soprano::Client::TcpClient::Private
{
public:
    Private()
        : connection( 0 ) {
    }

    TcpClientConnection* connection;
};


Soprano::Client::TcpClient::TcpClient( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    qRegisterMetaType<QAbstractSocket::SocketError>();
}


Soprano::Client::TcpClient::~TcpClient()
{
    disconnect();
    delete d;
}


bool Soprano::Client::TcpClient::connect( const QHostAddress& address, int port )
{
    if ( !d->connection ) {
        d->connection = new TcpClientConnection( address, port, this );
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


bool Soprano::Client::TcpClient::isConnected()
{
    return d->connection ? d->connection->testConnection() : false;
}


void Soprano::Client::TcpClient::disconnect()
{
    delete d->connection;
    d->connection = 0;
}


Soprano::Model* Soprano::Client::TcpClient::createModel( const QString& name, const QList<BackendSetting>& settings )
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


void Soprano::Client::TcpClient::removeModel( const QString& name )
{
    if ( d->connection ) {
        d->connection->removeModel( name );
        setError( d->connection->lastError() );
    }
    else {
        setError( "Not connected" );
    }
}


void Soprano::Client::TcpClient::slotError( QAbstractSocket::SocketError error )
{
    qDebug() << "Error: " << error;
}


// QStringList Soprano::Client::TcpClient::allModels() const
// {

// }

#include "tcpclient.moc"
