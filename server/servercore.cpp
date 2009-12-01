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
#include "soprano-server-config.h"
#include "serverconnection.h"
#ifdef BUILD_DBUS_SUPPORT
#include "dbus/dbuscontroller.h"
#endif
#include "modelpool.h"

#include "backend.h"
#include "storagemodel.h"
#include "global.h"
#include "asyncmodel.h"

#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QTcpSocket>
#include <QtDBus/QtDBus>


Q_DECLARE_METATYPE( QLocalSocket::LocalSocketError )
Q_DECLARE_METATYPE( QAbstractSocket::SocketError )


const quint16 Soprano::Server::ServerCore::DEFAULT_PORT = 5000;


class Soprano::Server::ServerCore::Private
{
public:
    Private()
        : maxConnectionCount( 0 ),
#ifdef BUILD_DBUS_SUPPORT
          dbusController( 0 ),
#endif
          tcpServer( 0 ),
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

    QTcpServer* tcpServer;
    QLocalServer* socketServer;

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

    void handleIncomingConnection( QIODevice* socket );

    void _s_localSocketError( QLocalSocket::LocalSocketError );
    void _s_tcpSocketError( QAbstractSocket::SocketError );
};


void Soprano::Server::ServerCore::Private::handleIncomingConnection( QIODevice* socket )
{
    qDebug() << Q_FUNC_INFO;
    if ( maxConnectionCount > 0 &&
         connections.count() >= maxConnectionCount ) {
        qDebug() << Q_FUNC_INFO << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA too many conenctions! go away!";
        delete socket;
    }
    else {
        ServerConnection* conn = new ServerConnection( modelPool, q );
        connections.append( conn );
        connect( conn, SIGNAL(finished()), q, SLOT(serverConnectionFinished()));
        connect( socket, SIGNAL( error( QLocalSocket::LocalSocketError ) ),
                 q, SLOT( _s_localSocketError( QLocalSocket::LocalSocketError ) ) );
        conn->start( socket );
        qDebug() << Q_FUNC_INFO << "New connection. New count:" << connections.count();
    }
}


void Soprano::Server::ServerCore::Private::_s_localSocketError( QLocalSocket::LocalSocketError error )
{
    if( error != QLocalSocket::PeerClosedError ) {
        qDebug() << "local socket error:" << error;
    }
}


void Soprano::Server::ServerCore::Private::_s_tcpSocketError( QAbstractSocket::SocketError error )
{
    qDebug() << "tcp socket error:" << error;
}



Soprano::Server::ServerCore::ServerCore( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    d->q = this;

    // default backend
    d->backend = Soprano::usedBackend();
    d->modelPool = new ModelPool( this );

    qRegisterMetaType<QLocalSocket::LocalSocketError>();
    qRegisterMetaType<QAbstractSocket::SocketError>();
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

        // Although the server is now multithreaded we still have the DBus interface which is not
        // It has its own dedicated thread but a deadlock can still be produced by a write operation
        // which is issued while an iterator is open. By using the AsyncModel in SingleThreadMode
        // we make sure that the DBus adaptor never deadlocks.

        Util::AsyncModel* asyncModel = new Util::AsyncModel( model );
        model->setParent( asyncModel ); // memory management
        d->models.insert( name, asyncModel );

        return asyncModel;
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
    clearError();
    if ( !d->tcpServer ) {
        d->tcpServer = new QTcpServer( this );
        connect( d->tcpServer, SIGNAL( newConnection() ),
                 this, SLOT( slotNewTcpConnection() ) );
    }

    if ( !d->tcpServer->listen( QHostAddress::Any, port ) ) {
        setError( QString( "Failed to start listening at port %1 on localhost." ).arg( port ) );
        qDebug() << "Failed to start listening at port " << port;
        return false;
    }
    else {
        qDebug() << "Listening on port " << port;
        return true;
    }
}


quint16 Soprano::Server::ServerCore::serverPort() const
{
    if ( d->tcpServer ) {
        return d->tcpServer->serverPort();
    }
    else {
        return 0;
    }
}


bool Soprano::Server::ServerCore::start( const QString& name )
{
    clearError();
    if ( !d->socketServer ) {
        d->socketServer = new QLocalServer( this );
        connect( d->socketServer, SIGNAL( newConnection() ),
                 this, SLOT( slotNewSocketConnection() ) );
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
    d->handleIncomingConnection( d->tcpServer->nextPendingConnection() );
}


void Soprano::Server::ServerCore::slotNewSocketConnection()
{
    qDebug() << Q_FUNC_INFO;
    d->handleIncomingConnection( d->socketServer->nextPendingConnection() );
}

#include "servercore.moc"
