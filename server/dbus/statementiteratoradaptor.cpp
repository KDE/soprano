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

#include "statementiteratoradaptor.h"
#include "dbusutil.h"
#include "iteratorwrapper.h"

Soprano::Server::DBusStatementIteratorAdaptor::DBusStatementIteratorAdaptor( IteratorWrapper* it )
    : QDBusAbstractAdaptor( it ),
      m_iteratorWrapper( it )
{
}

Soprano::Server::DBusStatementIteratorAdaptor::~DBusStatementIteratorAdaptor()
{
}

Soprano::Statement Soprano::Server::DBusStatementIteratorAdaptor::current( const QDBusMessage& m )
{
    // handle method call org.soprano.StatementIterator.current
    Soprano::Statement statement = m_iteratorWrapper->statementIterator().current();
    if ( m_iteratorWrapper->statementIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->statementIterator().lastError() );
    }
    return statement;
}

bool Soprano::Server::DBusStatementIteratorAdaptor::next( const QDBusMessage& m )
{
    // handle method call org.soprano.StatementIterator.next
    bool reply = m_iteratorWrapper->statementIterator().next();
    if ( m_iteratorWrapper->statementIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->statementIterator().lastError() );
    }
    return reply;
}

void Soprano::Server::DBusStatementIteratorAdaptor::close( const QDBusMessage& m )
{
    // handle method call org.soprano.StatementIterator.next
    m_iteratorWrapper->statementIterator().close();
    if ( m_iteratorWrapper->statementIterator().lastError() ) {
        DBus::sendErrorReply( m, m_iteratorWrapper->statementIterator().lastError() );
    }
}

#include "statementiteratoradaptor.moc"
