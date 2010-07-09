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
#include "tcpclient.h"
#include "clientconnection.h"
#include "clientmodel.h"

#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QPointer>
#include <QtCore/QtPlugin>

Q_EXPORT_PLUGIN2(soprano_serverclientbackend, Soprano::Client::ServerBackend)


class Soprano::Client::ServerBackend::Private
{
public:
    Private( ServerBackend* parent )
        : m_parent( parent ) {
    }

    // we keep one connection for each port
    QHash<quint16, QPointer<ClientConnection> > clientMap;
    QHash<ClientConnection*, QList<ClientModel*> > openModels;

    // FIXME: use TcpClient
    ClientConnection* ensureConnect( quint16 port ) {
//         QPointer<ClientConnection> client = clientMap[port];
//         if ( !client ) {
//             client = new ClientConnection( m_parent );
//             clientMap[port] = client;
//         }

//         if ( !client->isOpen() &&
//              !client->open( QHostAddress::LocalHost, port ) ) {
//             m_parent->setError( client->lastError() );
//             return 0;
//         }

//         return client;
#warning FIXME
    }

private:
    ServerBackend* m_parent;
};


Soprano::Client::ServerBackend::ServerBackend()
    : Backend( "sopranoserver" ),
      d ( new Private( this ) )
{
}


Soprano::Client::ServerBackend::~ServerBackend()
{
    delete d;
}


Soprano::StorageModel* Soprano::Client::ServerBackend::createModel( const QList<BackendSetting>& settings_ ) const
{
    QList<BackendSetting> settings( settings_ );

    // extract the port setting
    quint16 port = TcpClient::DEFAULT_PORT;
    QString name;
    QList<BackendSetting>::iterator it = settings.begin();
    while ( it != settings.end() ) {
        BackendSetting& setting = *it;

        if ( setting.option() == BackendOptionUser &&
             setting.userOptionName() == "port" ) {
            bool ok = true;
            port = setting.value().toString().toInt( &ok );
            if ( !ok ) {
                setError( QString( "Invalid value for option 'port': %1" ).arg( setting.value().toString() ), Error::ErrorInvalidArgument );
                return 0;
            }

            // remove the setting
            settings.erase( it );
        }

        else if ( setting.option() == BackendOptionUser &&
                  setting.userOptionName() == "name" ) {
            name = setting.value().toString();
            settings.erase( it );
        }
    }

    if ( name.isEmpty() ) {
        setError( "No Model name specified" );
        return 0;
    }

    ClientConnection* client = d->ensureConnect( port );
    if ( !client ) {
        return 0;
    }

    int modelId = client->createModel( name, settings );
    setError( client->lastError() );
    if ( modelId > 0 ) {
        ClientModel* model = new ClientModel( 0, modelId, client );
        connect( model, SIGNAL( destroyed(QObject*) ), this, SLOT( modelDeleted() ) );
        d->openModels[client].append( model );
        return model;
    }
    else {
        return 0;
    }
}


void Soprano::Client::ServerBackend::modelDeleted()
{
    ClientModel* model = qobject_cast<ClientModel*>( sender() );
    d->openModels[model->client()].removeAll( model );
    if ( d->openModels[model->client()].isEmpty() ) {
        delete model->client();
    }
}


Soprano::BackendFeatures Soprano::Client::ServerBackend::supportedFeatures() const
{
    // as we may have multiple connections, which one to choose?
    return BackendFeatureNone;
}

#include "serverbackend.moc"
