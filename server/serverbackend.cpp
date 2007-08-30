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

#include "serverbackend.h"
#include "client.h"
#include "clientmodel.h"

#include <QtCore/QtPlugin>
#include <QtCore/QList>


Q_EXPORT_PLUGIN2(soprano_serverclientbackend, Soprano::Server::ServerBackend)


const quint16 Soprano::Server::ServerBackend::DEFAULT_PORT = 5000;


class Soprano::Server::ServerBackend::Private
{
public:
    Client* client;
    quint16 port;

    QList<ClientModel*> openModels;

    bool ensureConnect() {
        if ( !client->isOpen() ) {
            return client->open( QHostAddress::LocalHost, port );
        }
        else {
            return true;
        }
    }
};


Soprano::Server::ServerBackend::ServerBackend( quint16 port )
    : Backend( "sopranoserver" ),
      d ( new Private() )
{
    d->client = new Client( this );
    d->port = port;
}


Soprano::Server::ServerBackend::~ServerBackend()
{
    Q_FOREACH( ClientModel* model, d->openModels ) {
        delete model;
    }
    delete d;
}


Soprano::StorageModel* Soprano::Server::ServerBackend::createModel( const QString& name ) const
{
    QList<BackendSetting> settings;
    settings.append( BackendSetting( "name", name ) );
    return createModel( settings );
}


Soprano::StorageModel* Soprano::Server::ServerBackend::createModel( const QList<BackendSetting>& settings ) const
{
    if ( !d->ensureConnect() ) {
        setError( d->client->lastError() );
        return 0;
    }

    int modelId = d->client->createModel( settings );
    setError( d->client->lastError() );
    StorageModel* model = new ClientModel( this, modelId, d->client );
    connect( model, SIGNAL( destroyed(QObject*) ), this, SLOT( modelDeleted() ) );
    return model;
}


Soprano::BackendFeatures Soprano::Server::ServerBackend::supportedFeatures() const
{
    if ( !d->ensureConnect() ) {
        setError( d->client->lastError() );
        return 0;
    }

    return d->client->supportedFeatures();
}


void Soprano::Server::ServerBackend::modelDeleted()
{
    d->openModels.removeAll( qobject_cast<ClientModel*>( sender() ) );
    if ( d->openModels.isEmpty() ) {
        d->client->close();
    }
}

#include "serverbackend.moc"
