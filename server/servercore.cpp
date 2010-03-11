/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2010 Sebastian Trueg <trueg@kde.org>
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
#include "soprano-server-config.h"
#include "serverconnection.h"
#include "socketserver.h"
#ifdef BUILD_DBUS_SUPPORT
#include "dbus/dbuscontroller.h"
#endif
#include "modelpool.h"

#include "backend.h"
#include "storagemodel.h"
#include "global.h"

#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtDBus/QtDBus>


const quint16 Soprano::Server::ServerCore::DEFAULT_PORT = 5000;


class Soprano::Server::ServerCore::Private
{
public:
    Private()
        : maxConnectionCount( 0 ),
#ifdef BUILD_DBUS_SUPPORT
          dbusController( 0 ),
#endif
          socketServer( 0 )
    {}

    const Backend* backend;
    BackendSettings settings;

    int maxConnectionCount;

    QHash<QString, Model*> models;
    QList<ServerConnection*> connections;

#ifdef BUILD_DBUS_SUPPORT
    DBusController* dbusController;
#endif

    LocalSocketServer* socketServer;

    // bridge between ServerCore and ServerConnection
    ModelPool* modelPool;

    ServerCore* q;

    BackendSettings createBackendSettings( const QString& name ) {
        BackendSettings newSettings = settings;
        if ( isOptionInSettings( newSettings, BackendOptionStorageDir ) ) {
            BackendSetting& setting = settingInSettings( newSettings, BackendOptionStorageDir );
            setting.setValue( setting.value().toString() + '/' + name );
        }
        return newSettings;
    }

    void handleIncomingConnection( SOCKET_HANDLE handle );
};


void Soprano::Server::ServerCore::Private::handleIncomingConnection( SOCKET_HANDLE handle )
{
    qDebug() << Q_FUNC_INFO;
    if ( maxConnectionCount > 0 &&
         connections.count() >= maxConnectionCount ) {
        qDebug() << Q_FUNC_INFO << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA too many conenctions! go away!";
    }
    else {
        ServerConnection* conn = new ServerConnection( modelPool, q );
        connections.append( conn );
        connect( conn, SIGNAL(finished()), q, SLOT(serverConnectionFinished()));
        conn->start( new Socket( handle ) );
        qDebug() << Q_FUNC_INFO << "New connection. New count:" << connections.count();
    }
}


Soprano::Server::ServerCore::ServerCore( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    d->q = this;

    // default backend
    d->backend = Soprano::usedBackend();
    d->modelPool = new ModelPool( this );
}


Soprano::Server::ServerCore::~ServerCore()
{
#ifdef BUILD_DBUS_SUPPORT
    delete d->dbusController;
#endif
    qDeleteAll( d->connections );
    qDeleteAll( d->models );
    delete d->modelPool;
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


QList<Soprano::BackendSetting> Soprano::Server::ServerCore::backendSettings() const
{
    return d->settings;
}


void Soprano::Server::ServerCore::setMaximumConnectionCount( int max )
{
    d->maxConnectionCount = max;
}


int Soprano::Server::ServerCore::maximumConnectionCount() const
{
    return d->maxConnectionCount;
}


Soprano::Model* Soprano::Server::ServerCore::model( const QString& name )
{
    QHash<QString, Model*>::const_iterator it = d->models.constFind( name );
    if ( it == d->models.constEnd() ) {
        BackendSettings settings = d->createBackendSettings( name );
        if ( isOptionInSettings( settings, BackendOptionStorageDir ) ) {
            QDir().mkpath( valueInSettings( settings, BackendOptionStorageDir ).toString() );
        }

        Model* model = createModel( settings );
        d->models.insert( name, model );

        return model;
    }
    else {
        return *it;
    }
}


void Soprano::Server::ServerCore::removeModel( const QString& name )
{
    clearError();

    QHash<QString, Model*>::iterator it = d->models.find( name );
    if ( it == d->models.end() ) {
        setError( QString( "Could not find model with name %1" ).arg( name ) );
    }
    else {
        Model* model = *it;
        d->models.erase( it );
        // delete the model, removing any cached data
        delete model;

        if ( isOptionInSettings( d->settings, BackendOptionStorageDir ) ) {
            // remove the data on disk
            backend()->deleteModelData( d->createBackendSettings( name ) );

            // remove the dir which should now be empty
            QDir( valueInSettings( d->settings, BackendOptionStorageDir ).toString() ).rmdir( name );
        }
    }
}


bool Soprano::Server::ServerCore::listen( quint16 port )
{
    setError( "TCP support not implemented" );
    return false;
}


quint16 Soprano::Server::ServerCore::serverPort() const
{
    setError( "TCP support not implemented" );
    return 0;
}


bool Soprano::Server::ServerCore::start( const QString& name )
{
    clearError();
    if ( !d->socketServer ) {
        d->socketServer = new LocalSocketServer( this );
        connect( d->socketServer, SIGNAL( newConnection(SOCKET_HANDLE) ),
                 this, SLOT( slotNewSocketConnection(SOCKET_HANDLE) ) );
    }

    QString path( name );
    if ( path.isEmpty() ) {
        path = QDir::homePath() + QLatin1String( "/.soprano/socket" );
    }

    if ( !d->socketServer->listen( path ) ) {
        setError( QString( "Failed to start listening at %1." ).arg( path ) );
        return false;
    }
    else {
        return true;
    }
}


void Soprano::Server::ServerCore::registerAsDBusObject( const QString& objectPath )
{
#ifdef BUILD_DBUS_SUPPORT
    if ( !d->dbusController ) {
        QString path( objectPath );
        if ( path.isEmpty() ) {
            path = "/org/soprano/Server";
        }
        d->dbusController = new Soprano::Server::DBusController( this, path );
    }
#else
    qFatal("Soprano has been built without D-Bus support!" );
#endif
}


void Soprano::Server::ServerCore::serverConnectionFinished()
{
    qDebug() << Q_FUNC_INFO;
    ServerConnection* conn = qobject_cast<ServerConnection*>( sender() );
    d->connections.removeAll( conn );
    delete conn;
    qDebug() << Q_FUNC_INFO << "Connection removed. Current count:" << d->connections.count();
}


Soprano::Model* Soprano::Server::ServerCore::createModel( const QList<BackendSetting>& settings )
{
    Model* m = backend()->createModel( settings );
    if ( m ) {
        clearError();
    }
    else if ( backend()->lastError() ) {
        setError( backend()->lastError() );
    }
    else {
        setError( "Could not create new Model for unknown reason" );
    }
    return m;
}


QStringList Soprano::Server::ServerCore::allModels() const
{
    return d->models.keys();
}


void Soprano::Server::ServerCore::slotNewTcpConnection()
{
    qDebug() << Q_FUNC_INFO;
    // FIXME
}


void Soprano::Server::ServerCore::slotNewSocketConnection( SOCKET_HANDLE handle )
{
    qDebug() << Q_FUNC_INFO;
    d->handleIncomingConnection( handle );
}

#include "servercore.moc"
