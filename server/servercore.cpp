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

#include "servercore.h"
#include "serverconnection.h"

#include <soprano/backend.h>
#include <soprano/storagemodel.h>
#include <soprano/global.h>

#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>


const quint16 Soprano::Server::ServerCore::DEFAULT_PORT = 5000;


class Soprano::Server::ServerCore::Private
{
public:
    const Backend* backend;
    QList<BackendSetting> settings;

    QHash<QString, Model*> models;
    QList<ServerConnection*> connections;
};


Soprano::Server::ServerCore::ServerCore()
    : d( new Private() )
{
    // default backend
    d->backend = Soprano::usedBackend();
}


Soprano::Server::ServerCore::~ServerCore()
{
    Q_FOREACH( ServerConnection* conn, d->connections ) {
        conn->close();
        conn->wait();
    }
    delete d;
}


void Soprano::Server::ServerCore::setBackend( const Backend* backend )
{
    d->backend = backend;
}


const Soprano::Backend* Soprano::Server::ServerCore::backend() const
{
    return d->backend;
}


void Soprano::Server::ServerCore::setBackendSettings( const QList<BackendSetting>& settings )
{
    d->settings = settings;
}


Soprano::Model* Soprano::Server::ServerCore::model( const QString& name )
{
    QHash<QString, Model*>::const_iterator it = d->models.find( name );
    if ( it == d->models.constEnd() ) {
        QList<BackendSetting> settings = d->settings;
        for ( QList<BackendSetting>::iterator it = settings.begin();
              it != settings.end(); ++it ) {
            BackendSetting& setting = *it;
            if ( setting.option() == BACKEND_OPTION_STORAGE_DIR ) {
                setting.setValue( setting.value().toString() + '/' + name );
            }
        }
        Model* model = createModel( settings );
        d->models.insert( name, model );
        return model;
    }
    else {
        return *it;
    }
}


bool Soprano::Server::ServerCore::start( quint16 port )
{
    clearError();
    if ( !listen( QHostAddress::LocalHost, port ) ) {
        setError( QString( "Failed to start listening at port %1 on localhost." ).arg( port ) );
        qDebug() << "Failed to start listening at port " << port;
        return false;
    }
    else {
        qDebug() << "Listening on port " << port;
        return true;
    }
}


void Soprano::Server::ServerCore::incomingConnection( int connection )
{
    qDebug() << "(ServerCore) new connection.";
    ServerConnection* conn = new ServerConnection( this, connection );
    d->connections.append( conn );
    connect( conn, SIGNAL(finished()), this, SLOT(serverConnectionFinished()));
    conn->start();
}


void Soprano::Server::ServerCore::serverConnectionFinished()
{
    ServerConnection* conn = qobject_cast<ServerConnection*>( sender() );
    d->connections.removeAll( conn );
    conn->deleteLater();
}


Soprano::Model* Soprano::Server::ServerCore::createModel( const QList<BackendSetting>& settings )
{
    return backend()->createModel( settings );
}

#include "servercore.moc"
