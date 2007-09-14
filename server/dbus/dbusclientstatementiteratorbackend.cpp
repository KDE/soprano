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

#include "dbusclientstatementiteratorbackend.h"
#include "statementiteratorinterface.h"
#include "util.h"

#include <soprano/statement.h>


Soprano::Client::DBusClientStatementIteratorBackend::DBusClientStatementIteratorBackend( const QString& serviceName, const QString& objectPath )
{
    m_interface = new DBusStatementIteratorInterface( serviceName, objectPath, QDBusConnection::sessionBus(), 0 );
}


Soprano::Client::DBusClientStatementIteratorBackend::~DBusClientStatementIteratorBackend()
{
    delete m_interface;
}


bool Soprano::Client::DBusClientStatementIteratorBackend::next()
{
    QDBusReply<bool> reply = m_interface->next();
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return false;
    }
    else {
        return reply.value();
    }
}


Soprano::Statement Soprano::Client::DBusClientStatementIteratorBackend::current() const
{
    QDBusReply<Statement> reply = m_interface->current();
    setError( DBus::convertError( reply.error() ) );
    return reply.value();
}


void Soprano::Client::DBusClientStatementIteratorBackend::close()
{
    QDBusReply<void> reply = m_interface->close();
    setError( DBus::convertError( reply.error() ) );
}
