/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2010-2012 Sebastian Trueg <trueg@kde.org>
 * Copyright (C) 2012      Vishesh Handa <me@vhanda.in>
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

#include "socket.h"

#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include <QtCore/QFile>
#include <QtCore/QDebug>

#ifndef Q_OS_WIN
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#else
#include <io.h>
#include <winsock2.h>
/* POSIX requires only at least 100 bytes */
#define UNIX_PATH_LEN   108

struct sockaddr_un {
  unsigned short sun_family;              /* address family AF_LOCAL/AF_UNIX */
  char           sun_path[UNIX_PATH_LEN]; /* 108 bytes of socket address     */
};
#endif

Soprano::Socket::Socket( SOCKET_HANDLE fd )
    : m_handle( fd ),
      m_mutex( QMutex::Recursive )
{
}


Soprano::Socket::~Socket()
{
    close();
}


bool Soprano::Socket::isConnected() const
{
    return m_handle > 0;
}


void Soprano::Socket::close()
{
    if ( m_handle >= 0 ) {
        ::close( m_handle );
        m_handle = -1;
    }
}


bool Soprano::Socket::waitForReadyRead( int timeout )
{
    if ( isConnected() ) {
        fd_set fds;
        FD_ZERO( &fds );
        FD_SET( m_handle, &fds );

        struct timeval tv;
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        int r = ::select( m_handle + 1, &fds, 0, 0, timeout < 0 ? 0 : &tv);
        if ( r == -1 ) {
            if ( errno == EINTR /* Interrupted system call */ )
                return waitForReadyRead( timeout );
        }

        return r > 0;
    }
    else {
        return false;
    }
}


qint64 Soprano::Socket::read( char* buffer, qint64 size )
{
    int total = 0;
    while ( size > 0 ) {
        int bytesRead = ::read( m_handle, buffer, size );
        if( bytesRead == -1 ) {
            if (errno == EINTR) {
                continue;
            }
            else {
                QString error = QString::fromLatin1( "Failed to read from fd %1 (%2)")
                                .arg( m_handle )
                                .arg( QLatin1String( strerror( errno) ) );
                setError( error );
                return -1;
            }
        }
        else if( bytesRead == 0 ) {
            QString error = QString::fromLatin1( "Timeout after reading %1 of %2 bytes" )
                            .arg( total ).arg( total + size );
            setError( error );
            break;
        }

        buffer += bytesRead;
        total += bytesRead;
        size -= bytesRead;
    }

    return total;
}


qint64 Soprano::Socket::write( const char* buffer, qint64 size )
{
    int total = 0;
    while ( size > 0 ) {
        int written = ::write( m_handle, buffer, size );
        if (written == -1) {
            if (errno == EINTR) {
                continue;
            }
            else {
                QString error = QString::fromLatin1( "Failed to write fd %1 (%2)")
                                .arg( m_handle )
                                .arg( QLatin1String( strerror( errno) ) );
                setError( error );
                return -1;
            }
        }
        else if( written == 0 ) {
            QString error = QString::fromLatin1( "Timeout after writing %1 of %2 bytes" )
                            .arg( total ).arg( total + size );
            setError( error );
            break;
        }

        buffer += written;
        total += written;
        size -= written;
    }

    return total;
}


void Soprano::Socket::lock()
{
    m_mutex.lock();
}


void Soprano::Socket::unlock()
{
    m_mutex.unlock();
}



Soprano::LocalSocket::LocalSocket()
{
}


Soprano::LocalSocket::~LocalSocket()
{
}


bool Soprano::LocalSocket::open( const QString& path )
{
    m_path = path;
    return open();
}


bool Soprano::LocalSocket::open()
{
    // create a socket
    m_handle = ::socket( AF_UNIX, SOCK_STREAM, 0 );
    if ( m_handle < 0 ) {
        setError( QString::fromLatin1( "Could not create socket (%1)" ).arg( strerror(errno) ) );
        return false;
    }

    // set the address
    struct sockaddr_un servAddr;
    memset( &servAddr, 0, sizeof(servAddr) );
    servAddr.sun_family = AF_UNIX;
    QByteArray s = QFile::encodeName( m_path );
    if ( s.length() > ( int )sizeof(servAddr.sun_path) + 1 ) {
        setError( QLatin1String( "Not enough space to store socket path." ) );
        ::close( m_handle );
        m_handle = -1;
        return false;
    }
    ::memcpy( servAddr.sun_path, s.data(), s.size()+1 );

    // connect to the server
    if ( ::connect( m_handle, (struct sockaddr *)&servAddr, sizeof( servAddr ) ) < 0 ) {
        setError( QString::fromLatin1( "Could not connect to server at %1 (%2)" ).arg( m_path ).arg( strerror(errno) ) );
        ::close( m_handle );
        m_handle = -1;
        return false;
    }

    return true;
}
