/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008-2012 Sebastian Trueg <trueg@kde.org>
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

#include "socketstream.h"

#include "node.h"
#include "statement.h"
#include "bindingset.h"
#include "backend.h"
#include "literalvalue.h"
#include "locator.h"
#include "languagetag.h"
#include <qdebug.h>

Soprano::SocketStream::SocketStream( Soprano::Socket* dev )
    : m_device( dev )
{
    m_device->lock();
}


Soprano::SocketStream::~SocketStream()
{
    m_device->unlock();
}


bool Soprano::SocketStream::read( char* data, qint64 size )
{
    if( size <= 0 )
        return true;

    m_device->read( data, size );
    Error::Error err = m_device->lastError();
    if( err ) {
        setError( err );
        return false;
    }

    return true;
}

bool Soprano::SocketStream::write(const char* data, qint64 len)
{
    if( len <= 0 )
        return true;

    m_device->write( data, len );
    Error::Error err = m_device->lastError();
    if( err ) {
        setError( err );
        return false;
    }

    return true;
}
