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

#include "socketdevice.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <poll.h>

#include <QtCore/QFile>
#include <QtCore/QDebug>


class SocketDevice::Private
{
public:
    Private()
        :sd( -1 ) {
    }
    int sd;
};


SocketDevice::SocketDevice( QObject* parent )
    : QIODevice( parent ),
      d( new Private() )
{
}


SocketDevice::~SocketDevice()
{
    delete d;
}


bool SocketDevice::open( int sd, OpenMode mode )
{
    close();
    if ( sd > 0 ) {
        d->sd = sd;
        return QIODevice::open( mode );
    }
    else {
        qDebug() << "Invalid file descriptor:" << sd;
        return false;
    }
}


bool SocketDevice::open( const QString& socketName, OpenMode mode )
{
    close();

    struct sockaddr_un servAddr;

    // create a socket
    d->sd = ::socket( AF_UNIX, SOCK_STREAM, 0 );
    if ( d->sd < 0 ) {
        setErrorString( QString( "Could not create socket (%1)" ).arg( strerror(errno) ) );
        return false;
    }

    // set the address
    memset( &servAddr, 0, sizeof(servAddr) );
    servAddr.sun_family = AF_UNIX;
    QByteArray s = QFile::encodeName( socketName );
    if ( s.length() > sizeof(servAddr.sun_path) + 1 ) {
        setErrorString( "Not enough space to store socket path." );
        ::close( d->sd );
        d->sd = -1;
        return false;
    }
    strncpy( servAddr.sun_path, s.data(), s.length() );
    servAddr.sun_path[s.length()] = '\0';

    // connect to the server
    if ( ::connect( d->sd, (struct sockaddr *)&servAddr, sizeof( servAddr ) ) < 0 ) {
        setErrorString( QString( "Could not connect to server at %1 (%2)" ).arg( socketName ).arg( strerror(errno) ) );
        ::close( d->sd );
        return false;
    }

    return QIODevice::open( mode );
}


void SocketDevice::close()
{
    if ( d->sd >= 0 ) {
        ::close( d->sd );
        d->sd = -1;
        QIODevice::close();
    }
}


qint64 SocketDevice::readData( char* data, qint64 maxSize )
{
    return ::read( d->sd, data, maxSize );
}


qint64 SocketDevice::writeData( const char* data, qint64 maxSize )
{
    return ::write( d->sd, data, maxSize );
}


bool SocketDevice::waitForReadyRead( int msecs )
{
    struct pollfd fd[1];
    fd[0].fd = d->sd;
    fd[0].events = POLLIN;
    int r = ::poll( fd, 1, msecs );
    if ( r > 0 ) {
        if ( fd[0].revents == POLLIN ) {
            return true;
        }
        else {
            if ( fd[0].revents & POLLHUP ) {
                qDebug() << "Connection remotely closed.";
                close();
            }

            return false;
        }
    }
    else {
        if ( r < 0 ) {
            qDebug() << strerror( errno );
            setErrorString( "Connection broken" );
            close();
        }
        else {
            setErrorString( "wait for read timeout" );
        }
        return false;
    }
}


bool SocketDevice::isValid()
{
    return d->sd > 0;
}
