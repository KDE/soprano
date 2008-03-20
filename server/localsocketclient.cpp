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
#include <QtNetwork/QLocalSocket>


class Soprano::Client::LocalSocketClient::Private
{
public:
    ClientConnection connection;
    QLocalSocket socket;
};


Soprano::Client::LocalSocketClient::LocalSocketClient( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    d->connection.connect( &d->socket );
}


Soprano::Client::LocalSocketClient::~LocalSocketClient()
{
    disconnect();
    delete d;
}


bool Soprano::Client::LocalSocketClient::connect( const QString& name )
{
    if ( !d->socket.isValid() ) {
        QString path( name );
        if ( path.isEmpty() ) {
            path = QDir::homePath() + QLatin1String( "/.soprano/socket" );
        }

        d->socket.connectToServer( path, QIODevice::ReadWrite );
        if ( !d->socket.waitForConnected() ) {
            setError( d->socket.errorString() );
            return false;
        }
        else {
            if ( !d->connection.checkProtocolVersion() ) {
                disconnect();
                return false;
            }
            else {
                return true;
            }
        }
    }
    else {
        setError( "Already connected" );
        return false;
    }
}


bool Soprano::Client::LocalSocketClient::isConnected()
{
    return d->socket.isValid();
}


void Soprano::Client::LocalSocketClient::disconnect()
{
    d->socket.close();
}


Soprano::Model* Soprano::Client::LocalSocketClient::createModel( const QString& name, const QList<BackendSetting>& settings )
{
    int modelId = d->connection.createModel( name, settings );
    setError( d->connection.lastError() );
    if ( modelId > 0 ) {
        StorageModel* model = new ClientModel( 0, modelId, &d->connection );
        return model;
    }
    else {
        return 0;
    }
}


void Soprano::Client::LocalSocketClient::removeModel( const QString& name )
{
    d->connection.removeModel( name );
    setError( d->connection.lastError() );
}

#include "localsocketclient.moc"
