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


class Soprano::Server::BackendPlugin::Private
{
public:
    Client* client;
};


Soprano::Server::BackendPlugin::BackendPlugin()
    : Backend( "sopranoserver" ),
      d ( new Private() )
{
    d->client = new Client( this );
}


Soprano::Server::BackendPlugin::~BackendPlugin()
{
    delete d;
}


Soprano::StorageModel* Soprano::Server::BackendPlugin::createModel( const QString& name ) const
{
    QList<BackendSetting> settings;
    settings.append( BackendSetting( "name", name ) );
    return createModel( settings );
}


Soprano::StorageModel* Soprano::Server::BackendPlugin::createModel( const QList<BackendSetting>& settings ) const
{
    int modelId = d->client->createModel( settings );
    setError( d->client->lastError() );
    return new ClientModel( this, modelId, d->client );
}


Soprano::BackendFeatures Soprano::Server::BackendPlugin::supportedFeatures() const
{
    return d->client->supportedFeatures();
}

#include "serverbackend.moc"
