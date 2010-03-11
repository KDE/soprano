/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>
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

#include "socketnotifier.h"

#include <QtCore/QSocketNotifier>

namespace {
    QSocketNotifier::Type typeToType( Soprano::SocketNotifier::Type type )
    {
        switch( type ) {
        case Soprano::SocketNotifier::Read:
            return QSocketNotifier::Read;
        case Soprano::SocketNotifier::Write:
            return QSocketNotifier::Write;
        default:
            return QSocketNotifier::Exception;
        }
    }
}

Soprano::SocketNotifier::SocketNotifier( Socket* socket, Type type )
    : m_socket( socket )
{
    m_socketNotifier = new QSocketNotifier( m_socket->handle(), typeToType( type ), this );
    connect( m_socketNotifier, SIGNAL( activated( int ) ),
             this, SIGNAL( activated( int ) ) );
}


Soprano::SocketNotifier::~SocketNotifier()
{
}

#include "socketnotifier.moc"
