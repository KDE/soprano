/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2012 Sebastian Trueg <trueg@kde.org>
 * Copyright (C) 2012      Vishesh Handa <me@vhanda.in>
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
#include "socket.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QThread>
#include <QtNetwork/QLocalSocket>

namespace Soprano {
    namespace Client {
        class LocalSocketClientConnection : public ClientConnection
        {
        public:
            LocalSocketClientConnection( QObject* parent = 0 );
            ~LocalSocketClientConnection();

            void setSocketPath( const QString& path ) {
                m_socketPath = path;
            }

            virtual bool connect();
            virtual bool disconnect();

            virtual bool isConnected() {
                return m_localSocket.isConnected();
            }
        protected:
            virtual Socket* getSocket() {
                return &m_localSocket;
            }

        private:
            QString m_socketPath;
            LocalSocket m_localSocket;
        };

        LocalSocketClientConnection::LocalSocketClientConnection( QObject* parent )
            : ClientConnection( parent )
        {
        }

        LocalSocketClientConnection::~LocalSocketClientConnection()
        {
        }

        bool LocalSocketClientConnection::connect()
        {
            if( m_localSocket.isConnected() ) {
                setError( "Already connected" );
                return false;
            }

            if( m_socketPath.isEmpty() ) {
                m_socketPath = QDir::homePath() + QLatin1String( "/.soprano/socket" );
            }

            if ( !m_localSocket.open( m_socketPath ) ) {
                setError( m_localSocket.lastError() );
                return false;
            }

            return true;
        }

        bool LocalSocketClientConnection::disconnect()
        {
            if( m_localSocket.isConnected() ) {
                m_localSocket.close();
                return true;
            }
            return false;
        }
    }
}


class Soprano::Client::LocalSocketClient::Private
{
public:
    Private()
        : connection( 0 ) {
    }

    LocalSocketClientConnection connection;
};

Soprano::Client::LocalSocketClient::LocalSocketClient( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
}


Soprano::Client::LocalSocketClient::~LocalSocketClient()
{
    disconnect();
    delete d;
}


bool Soprano::Client::LocalSocketClient::connect( const QString& name )
{
    if ( !isConnected() ) {
        d->connection.setSocketPath( name );
        if ( d->connection.connect() &&
             d->connection.checkProtocolVersion() ) {
            return true;
        }
        else {
            setError( d->connection.lastError() );
            return false;
        }
    }
    else {
        setError( "Already connected" );
        return true;
    }
}


bool Soprano::Client::LocalSocketClient::isConnected() const
{
    return d->connection.isConnected();
}


void Soprano::Client::LocalSocketClient::disconnect()
{
    d->connection.disconnect();
}


Soprano::Model* Soprano::Client::LocalSocketClient::createModel( const QString& name, const QList<BackendSetting>& settings )
{
    if ( d->connection.isConnected() ) {
        int modelId = d->connection.createModel( name, settings );
        setError( d->connection.lastError() );
        if ( modelId > 0 ) {
            StorageModel* model = new ClientModel( 0, modelId, &d->connection );
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
    if ( d->connection.isConnected() ) {
        d->connection.removeModel( name );
        setError( d->connection.lastError() );
    }
    else {
        setError( "Not connected" );
    }
}

