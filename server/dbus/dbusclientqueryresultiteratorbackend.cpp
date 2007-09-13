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

#include "dbusclientqueryresultiteratorbackend.h"
#include "queryresultiteratorinterface.h"
#include "util.h"

#include <soprano/statement.h>
#include <soprano/bindingset.h>


Soprano::Server::DBusClientQueryResultIteratorBackend::DBusClientQueryResultIteratorBackend( const QString& serviceName, const QString& objectPath )
{
    m_interface = new DBusQueryResultIteratorInterface( serviceName, objectPath, QDBusConnection::sessionBus(), 0 );
}


Soprano::Server::DBusClientQueryResultIteratorBackend::~DBusClientQueryResultIteratorBackend()
{
    delete m_interface;
}


bool Soprano::Server::DBusClientQueryResultIteratorBackend::next()
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


Soprano::BindingSet Soprano::Server::DBusClientQueryResultIteratorBackend::current() const
{
    QDBusReply<BindingSet> reply = m_interface->current();
    setError( DBus::convertError( reply.error() ) );
    return reply.value();
}


void Soprano::Server::DBusClientQueryResultIteratorBackend::close()
{
    QDBusReply<void> reply = m_interface->close();
    setError( DBus::convertError( reply.error() ) );
}


Soprano::Statement Soprano::Server::DBusClientQueryResultIteratorBackend::currentStatement() const
{
    QDBusReply<Statement> reply = m_interface->currentStatement();
    setError( DBus::convertError( reply.error() ) );
    return reply.value();
}


Soprano::Node Soprano::Server::DBusClientQueryResultIteratorBackend::binding( const QString &name ) const
{
    QDBusReply<Node> reply = m_interface->bindingByName( name );
    setError( DBus::convertError( reply.error() ) );
    return reply.value();
}


Soprano::Node Soprano::Server::DBusClientQueryResultIteratorBackend::binding( int offset ) const
{
    QDBusReply<Node> reply = m_interface->bindingByIndex( offset );
    setError( DBus::convertError( reply.error() ) );
    return reply.value();
}


int Soprano::Server::DBusClientQueryResultIteratorBackend::bindingCount() const
{
    QDBusReply<int> reply = m_interface->bindingCount();
    setError( DBus::convertError( reply.error() ) );
    return reply.value();
}


QStringList Soprano::Server::DBusClientQueryResultIteratorBackend::bindingNames() const
{
    QDBusReply<QStringList> reply = m_interface->bindingNames();
    setError( DBus::convertError( reply.error() ) );
    return reply.value();
}


bool Soprano::Server::DBusClientQueryResultIteratorBackend::isGraph() const
{
    QDBusReply<bool> reply = m_interface->isGraph();
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return false;
    }
    else {
        return reply.value();
    }
}


bool Soprano::Server::DBusClientQueryResultIteratorBackend::isBinding() const
{
    QDBusReply<bool> reply = m_interface->isBinding();
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return false;
    }
    else {
        return reply.value();
    }
}


bool Soprano::Server::DBusClientQueryResultIteratorBackend::isBool() const
{
    QDBusReply<bool> reply = m_interface->isBool();
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return false;
    }
    else {
        return reply.value();
    }
}


bool Soprano::Server::DBusClientQueryResultIteratorBackend::boolValue() const
{
    QDBusReply<bool> reply = m_interface->boolValue();
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return false;
    }
    else {
        return reply.value();
    }
}
