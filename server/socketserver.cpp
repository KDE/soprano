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

#include "socketserver.h"
#include "socketnotifier.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <QtCore/QFile>
#include <QtCore/QDebug>



Soprano::SocketServer::SocketServer( QObject* parent )
    : QObject( parent ),
      m_socket( 0 ),
      m_socketNotifier( 0 )
{
}


Soprano::SocketServer::~SocketServer()
{
    close();
}



void Soprano::SocketServer::close()
{
    delete m_socket;
    m_socket = 0;
    delete m_socketNotifier;
    m_socketNotifier = 0;
}


void Soprano::SocketServer::handleOpenedServerSocket( SOCKET_HANDLE handle )
{
    m_socket = new Socket( handle );
    m_socketNotifier = new SocketNotifier( m_socket, SocketNotifier::Read );
    connect( m_socketNotifier, SIGNAL( activated( int ) ), this, SLOT( slotServerSocketActivated() ) );
}


void Soprano::SocketServer::slotServerSocketActivated()
{
    clearError();
    if ( m_socket->handle() > 0 ) {
        SOCKET_HANDLE sd = ::accept( m_socket->handle(), 0, 0 );
        if ( sd <= 0 ) {
            setError( QString( "Failed to accept new connection (%1)" ).arg( strerror( errno ) ) );
        }
        else {
            emit newConnection( sd );
        }
    }
    else {
        setError( "Not connected" );
    }
}


Soprano::LocalSocketServer::LocalSocketServer( QObject* parent )
    : SocketServer( parent )
{
}


Soprano::LocalSocketServer::~LocalSocketServer()
{
}


bool Soprano::LocalSocketServer::listen( const QString& socketName )
{
    close();

    /* create new socket */
    SOCKET_HANDLE handle = ::socket( AF_UNIX, SOCK_STREAM, 0 );
    if( handle < 0 ) {
        setError( "Unable to create socket." );
        return false;
    }

    /* set the address */
    struct sockaddr_un sock;
    QByteArray s = QFile::encodeName( socketName );
    if ( s.length() > ( int )sizeof(sock.sun_path) + 1 ) {
        setError( "Not enough space to store socket path." );
        ::close( handle );
        return false;
    }
    strncpy( sock.sun_path, s.data(), s.length() );
    sock.sun_path[s.length()] = '\0';

    /* bind server port */
    sock.sun_family = AF_UNIX;
    if ( ::bind( handle, (struct sockaddr *)&sock, sizeof(sock) ) < 0 ) {
        setError( QString( "Unable to bind socket %1 (%2)" ).arg( handle ).arg( strerror( errno ) ) );
        ::close( handle );
        return false;
    }

    if ( ::listen( handle, 5 ) < 0 ) {
        setError( QString( "Unable to listen on socket %1 (%2)" ).arg( handle ).arg( strerror( errno ) ) );
        ::close( handle );
        QFile::remove( socketName );
        return false;
    }

    m_socketName = socketName;

    handleOpenedServerSocket( handle );

    return true;
}


void Soprano::LocalSocketServer::close()
{
    SocketServer::close();
    if ( !m_socketName.isEmpty() &&
         QFile::exists( m_socketName ) )
        QFile::remove( m_socketName );
}

#include "socketserver.moc"
