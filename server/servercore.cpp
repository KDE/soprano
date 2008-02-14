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
#include "socketserver.h"
#include "socketdevice.h"
#include "dbus/dbusserveradaptor.h"

#include "soprano-server-config.h"

#include "backend.h"
#include "storagemodel.h"
#include "global.h"

#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>


const quint16 Soprano::Server::ServerCore::DEFAULT_PORT = 5000;


class Soprano::Server::ServerCore::Private
{
public:
    Private()
        : dbusAdaptor( 0 ),
          tcpServer( 0 )
#ifdef HAVE_UNIX_SOCKETS
          , socketServer( 0 )
#endif
    {}

    const Backend* backend;
    QList<BackendSetting> settings;

    QHash<QString, Model*> models;
    QList<ServerConnection*> connections;

    DBusServerAdaptor* dbusAdaptor;

    QTcpServer* tcpServer;
#ifdef HAVE_UNIX_SOCKETS
    SocketServer* socketServer;
#endif
};


Soprano::Server::ServerCore::ServerCore( QObject* parent )
    : QObject( parent ),
      d( new Private() )
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
    qDeleteAll( d->connections );
    qDeleteAll( d->models );
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


Soprano::Model* Soprano::Server::ServerCore::model( const QString& name )
{
    QHash<QString, Model*>::const_iterator it = d->models.find( name );
    if ( it == d->models.constEnd() ) {
        QList<BackendSetting> settings = d->settings;
        for ( QList<BackendSetting>::iterator it = settings.begin();
              it != settings.end(); ++it ) {
            BackendSetting& setting = *it;
            if ( setting.option() == BackendOptionStorageDir ) {
                setting.setValue( setting.value().toString() + '/' + name );
                QDir().mkpath( setting.value().toString() );
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


namespace {
    void removeFile( const QString& path )
    {
        qDebug() << "Removing" << path;
        QDir dir( path );
        if ( dir.exists() ) {
            QStringList children = dir.entryList();
            children.removeAll( "." );
            children.removeAll( ".." );
            Q_FOREACH( QString s, children ) {
                removeFile( dir.filePath( s ) );
            }

            dir.rmpath( path );
        }
        else {
            QFile::remove( path );
        }
    }
}

void Soprano::Server::ServerCore::removeModel( const QString& name )
{
    clearError();

    QHash<QString, Model*>::iterator it = d->models.find( name );
    if ( it == d->models.constEnd() ) {
        setError( QString( "Could not find model with name %1" ).arg( name ) );
    }
    else {
        Model* model = *it;
        d->models.erase( it );
        QString dir;
        for ( QList<BackendSetting>::const_iterator it = d->settings.begin();
              it != d->settings.end(); ++it ) {
            const BackendSetting& setting = *it;
            if ( setting.option() == BackendOptionStorageDir ) {
                dir = setting.value().toString() + '/' + name;
                break;
            }
        }

        // delete the model, removing any cached data
        delete model;

        // remove the data on disk
        if ( !dir.isEmpty() ) {
            removeFile( dir );
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

    if ( !d->tcpServer->listen( QHostAddress::LocalHost, port ) ) {
        setError( QString( "Failed to start listening at port %1 on localhost." ).arg( port ) );
        qDebug() << "Failed to start listening at port " << port;
        return false;
    }
    else {
        qDebug() << "Listening on port " << port;
        return true;
    }
}


bool Soprano::Server::ServerCore::start( const QString& name )
{
#ifdef HAVE_UNIX_SOCKETS
    clearError();
    if ( !d->socketServer ) {
        d->socketServer = new SocketServer( this );
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
#else
    setError( QLatin1String( "Unix socket communication not supported." ) );
    return false;
#endif
}


void Soprano::Server::ServerCore::registerAsDBusObject( const QString& objectPath )
{
    if ( !d->dbusAdaptor ) {
        QString path( objectPath );
        if ( path.isEmpty() ) {
            path = "/org/soprano/Server";
        }

        d->dbusAdaptor = new Soprano::Server::DBusServerAdaptor( this, path );
        QDBusConnection::sessionBus().registerObject( path, this );
    }
}


void Soprano::Server::ServerCore::serverConnectionFinished()
{
    ServerConnection* conn = qobject_cast<ServerConnection*>( sender() );
    d->connections.removeAll( conn );
    conn->deleteLater();
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
    qDebug() << "(ServerCore) new tcp connection.";
    ServerConnection* conn = new ServerConnection( this );
    d->connections.append( conn );
    connect( conn, SIGNAL(finished()), this, SLOT(serverConnectionFinished()));
    conn->start( d->tcpServer->nextPendingConnection() );
}


void Soprano::Server::ServerCore::slotNewSocketConnection()
{
#ifdef HAVE_UNIX_SOCKETS
    qDebug() << "(ServerCore) new socket connection.";
    ServerConnection* conn = new ServerConnection( this );
    d->connections.append( conn );
    connect( conn, SIGNAL(finished()), this, SLOT(serverConnectionFinished()));
    conn->start( d->socketServer->nextPendingConnection() );
#endif
}

#include "servercore.moc"
