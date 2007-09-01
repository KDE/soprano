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

#include "client.h"
#include "clientconnection.h"
#include "clientmodel.h"

const quint16 Soprano::Server::Client::DEFAULT_PORT = 5000;


class Soprano::Server::Client::Private
{
public:
    ClientConnection connection;
};


Soprano::Server::Client::Client( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
}


Soprano::Server::Client::~Client()
{
    disconnect();
    delete d;
}


bool Soprano::Server::Client::connect( int port )
{
    if ( !d->connection.isOpen() ) {
        return d->connection.open( QHostAddress::LocalHost, port );
    }
    else {
        setError( "Already connected" );
        return false;
    }
}


bool Soprano::Server::Client::isConnected()
{
    return d->connection.isOpen();
}


void Soprano::Server::Client::disconnect()
{
    d->connection.close();
}


Soprano::Model* Soprano::Server::Client::createModel( const QString& name, const QList<BackendSetting>& settings )
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


// QStringList Soprano::Server::Client::models() const
// {

// }

#include "client.moc"
