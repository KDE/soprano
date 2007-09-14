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

#include "socketserver.h"
#include "socketdevice.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>

#include <QtCore/QFile>
#include <QtCore/QLinkedList>
#include <QtCore/QDebug>
#include <QtCore/QSocketNotifier>


class SocketServer::Private
{
public:
    Private()
        : serverSocket( -1 ),
          notifier( 0 ) {
    }

    QString socketName;
    int serverSocket;
    QSocketNotifier* notifier;

    QLinkedList<int> connectionCache;
};


SocketServer::SocketServer( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
}


SocketServer::~SocketServer()
{
    close();
    delete d;
}


bool SocketServer::listen( const QString& socketName )
{
    close();

    /* create new socket */
    d->serverSocket = ::socket( AF_UNIX, SOCK_STREAM, 0 );
    if( d->serverSocket < 0 ) {
        setError( "Unable to create socket." );
        return false;
    }

    /* set the address */
    struct sockaddr_un sock;
    QByteArray s = QFile::encodeName( socketName );
    if ( s.length() > sizeof(sock.sun_path) + 1 ) {
        setError( "Not enough space to store socket path." );
        ::close( d->serverSocket );
        d->serverSocket = -1;
        return false;
    }
    strncpy( sock.sun_path, s.data(), s.length() );
    sock.sun_path[s.length()] = '\0';

    /* bind server port */
    sock.sun_family = AF_UNIX;
    if ( ::bind( d->serverSocket, (struct sockaddr *)&sock, sizeof(sock) ) < 0 ) {
        setError( QString( "Unable to bind socket %1 (%2)" ).arg( d->serverSocket ).arg( strerror( errno ) ) );
        ::close( d->serverSocket );
        d->serverSocket = -1;
        return false;
    }

    if ( ::listen( d->serverSocket, 5 ) < 0 ) {
        setError( QString( "Unable to listen on socket %1 (%2)" ).arg( d->serverSocket ).arg( strerror( errno ) ) );
        ::close( d->serverSocket );
        d->serverSocket = -1;
        QFile::remove( socketName );
        return false;
    }

    d->socketName = socketName;
    d->notifier = new QSocketNotifier( d->serverSocket, QSocketNotifier::Read, this );
    connect( d->notifier, SIGNAL( activated( int ) ), this, SLOT( slotServerSocketActivated() ) );

    return true;
}

void SocketServer::close()
{
    if ( d->serverSocket > 0 ) {
        delete d->notifier;
        d->notifier = 0;
        ::close( d->serverSocket );
        d->serverSocket = -1;
        QFile::remove( d->socketName );
    }
}


QString SocketServer::socketName() const
{
    return d->socketName;
}


SocketDevice* SocketServer::nextPendingConnection()
{
    if ( !d->connectionCache.isEmpty() ) {
        int sd = d->connectionCache.takeFirst();
        SocketDevice* dev = new SocketDevice();
        dev->open( sd, QIODevice::ReadWrite );
        return dev;
    }
    else {
        setError( "No pending connections" );
        return 0;
    }
}


void SocketServer::incomingConnection( int socketDescriptor )
{
    d->connectionCache.append( socketDescriptor );
    emit newConnection();
}


void SocketServer::slotServerSocketActivated()
{
    clearError();
    if ( d->serverSocket > 0 ) {
        int sd = ::accept( d->serverSocket, 0, 0 );
        if ( sd <= 0 ) {
            setError( QString( "Failed to accept new connection (%1)" ).arg( strerror( errno ) ) );
        }
        else {
            incomingConnection( sd );
        }
    }
    else {
        setError( "Not connected" );
    }
}

#include "socketserver.moc"
