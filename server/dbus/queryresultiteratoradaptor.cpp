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

#include "queryresultiteratoradaptor.h"
#include "dbusutil.h"
#include "iteratorwrapper.h"

#include <soprano/node.h>
#include <soprano/statement.h>
#include <soprano/bindingset.h>


Soprano::Server::DBusQueryResultIteratorAdaptor::DBusQueryResultIteratorAdaptor( IteratorWrapper* it )
    : QDBusAbstractAdaptor( it ),
      m_iteratorWrapper( it )
{
}

Soprano::Server::DBusQueryResultIteratorAdaptor::~DBusQueryResultIteratorAdaptor()
{
}

Soprano::BindingSet Soprano::Server::DBusQueryResultIteratorAdaptor::current( const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.current
    Soprano::BindingSet set = m_iteratorWrapper->queryResultIterator().current();
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
    return set;
}

Soprano::Statement Soprano::Server::DBusQueryResultIteratorAdaptor::currentStatement( const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.currentStatement
    Soprano::Statement s = m_iteratorWrapper->queryResultIterator().currentStatement();
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
    return s;
}

bool Soprano::Server::DBusQueryResultIteratorAdaptor::next( const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.next
    bool reply = m_iteratorWrapper->queryResultIterator().next();
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
    return reply;
}

Soprano::Node Soprano::Server::DBusQueryResultIteratorAdaptor::bindingByIndex( int index, const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.bindingByIndex
    Soprano::Node node = m_iteratorWrapper->queryResultIterator().binding( index );
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
    return node;
}

Soprano::Node Soprano::Server::DBusQueryResultIteratorAdaptor::bindingByName( const QString& name, const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.bindingByName
    Soprano::Node node = m_iteratorWrapper->queryResultIterator().binding( name );
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
    return node;
}

int Soprano::Server::DBusQueryResultIteratorAdaptor::bindingCount( const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.bindingCount
    int cnt = m_iteratorWrapper->queryResultIterator().bindingCount();
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
    return cnt;
}

QStringList Soprano::Server::DBusQueryResultIteratorAdaptor::bindingNames( const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.bindingNames
    QStringList names = m_iteratorWrapper->queryResultIterator().bindingNames();
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
    return names;
}

bool Soprano::Server::DBusQueryResultIteratorAdaptor::boolValue( const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.boolValue
    bool reply = m_iteratorWrapper->queryResultIterator().boolValue();
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
    return reply;
}

bool Soprano::Server::DBusQueryResultIteratorAdaptor::isBinding( const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.isBinding
    bool reply = m_iteratorWrapper->queryResultIterator().isBinding();
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
    return reply;
}

bool Soprano::Server::DBusQueryResultIteratorAdaptor::isBool( const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.isBool
    bool reply = m_iteratorWrapper->queryResultIterator().isBool();
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
    return reply;
}

bool Soprano::Server::DBusQueryResultIteratorAdaptor::isGraph( const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.isGraph
    bool reply = m_iteratorWrapper->queryResultIterator().isGraph();
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
    return reply;
}

void Soprano::Server::DBusQueryResultIteratorAdaptor::close( const QDBusMessage& m )
{
    // handle method call org.soprano.QueryResultIterator.current
    m_iteratorWrapper->queryResultIterator().close();
    if ( m_iteratorWrapper->queryResultIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->queryResultIterator().lastError() );
    }
}

#include "queryresultiteratoradaptor.moc"
