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

#include "unixsocketclient.h"
#include "clientconnection.h"
#include "clientmodel.h"
#include "socketdevice.h"

#include <QtCore/QDir>


class Soprano::Client::UnixSocketClient::Private
{
public:
    ClientConnection connection;
    SocketDevice socket;
};


Soprano::Client::UnixSocketClient::UnixSocketClient( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    d->connection.connect( &d->socket );
}


Soprano::Client::UnixSocketClient::~UnixSocketClient()
{
    disconnect();
    delete d;
}


bool Soprano::Client::UnixSocketClient::connect( const QString& name )
{
    if ( !d->socket.isValid() ) {
        QString path( name );
        if ( path.isEmpty() ) {
            path = QDir::homePath() + QLatin1String( "/.soprano/socket" );
        }

        if ( !d->socket.open( path, QIODevice::ReadWrite ) ) {
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


bool Soprano::Client::UnixSocketClient::isConnected()
{
    return d->socket.isValid();
}


void Soprano::Client::UnixSocketClient::disconnect()
{
    d->socket.close();
}


Soprano::Model* Soprano::Client::UnixSocketClient::createModel( const QString& name, const QList<BackendSetting>& settings )
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


// QStringList Soprano::Client::UnixSocketClient::models() const
// {

// }

#include "unixsocketclient.moc"
