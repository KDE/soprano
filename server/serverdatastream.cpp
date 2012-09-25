/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "serverdatastream.h"

#include <QtCore/QIODevice>


Soprano::Server::DataStream::DataStream( QIODevice* dev )
    : m_device( dev )
{
}


Soprano::Server::DataStream::~DataStream()
{
}


bool Soprano::Server::DataStream::read( char* data, qint64 size )
{
    qint64 cnt = 0;
    while ( cnt < size ) {
        qint64 m = qMin( size-cnt, 1024LL );
        qint64 r = m_device->read( data+cnt, m );
        if ( r < 0 ) {
            setError( Error::Error( QString( "Failed to read after %1 of %2 bytes (%3)." )
                                    .arg( cnt )
                                    .arg( size )
                                    .arg( m_device->errorString() ) ) );
            return false;
        }
        else if ( r == 0 && size > 0 ) {
            if ( !m_device->waitForReadyRead( 30000 ) ) {
                setError( Error::Error( QString( "Timeout when reading after %1 of %2 bytes (%3)." )
                                        .arg( cnt )
                                        .arg( size )
                                        .arg( m_device->errorString() ) ) );
                return false;
            }
        }

        cnt += r;
    }

    // vHanda: Is this clearError() really required?
    // clearError();
    return true;
}


bool Soprano::Server::DataStream::write(const char* data, qint64 len)
{
    quint32 cnt = 0;
    while ( cnt < len ) {
        int x = qMin( (qint64)1024, len-cnt );
        int r = m_device->write( data+cnt, x );
        if ( r < 0 ) {
            setError( Error::Error( QString( "Failed to write string after %1 of %2 bytes (%3)." ).arg( cnt ).arg( len ).arg( m_device->errorString() ) ) );
            return false;
        }
        cnt += r;
    }

    return true;
}


