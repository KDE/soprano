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

#include "multicallprotectionqueryresultiteratorbackend.h"
#include "multicallprotectionmodel.h"

#include "error.h"
#include "statement.h"


Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::MultiCallProtectionQueryResultIteratorBackend( const QueryResultIterator& it, MultiCallProtectionModel* model )
    : QueryResultIteratorBackend(),
      MultiCallProtectionIteratorBase( model ),
      m_iterator( it )
{
}


Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::~MultiCallProtectionQueryResultIteratorBackend()
{
}


bool Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::next()
{
    bool haveNext = m_iterator.next();
    setError( m_iterator.lastError() );
    return haveNext;
}


Soprano::BindingSet Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::current() const
{
    BindingSet s = m_iterator.current();
    setError( m_iterator.lastError() );
    return s;
}


void Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::close()
{
    m_iterator.close();
    setError( m_iterator.lastError() );
    remove();
}


Soprano::Statement Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::currentStatement() const
{
    Statement s = m_iterator.currentStatement();
    setError( m_iterator.lastError() );
    return s;
}


Soprano::Node Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::binding( const QString &name ) const
{
    Node n = m_iterator.binding( name );
    setError( m_iterator.lastError() );
    return n;
}


Soprano::Node Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::binding( int offset ) const
{
    Node n = m_iterator.binding( offset );
    setError( m_iterator.lastError() );
    return n;
}


int Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::bindingCount() const
{
    int cnt = m_iterator.bindingCount();
    setError( m_iterator.lastError() );
    return cnt;
}


QStringList Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::bindingNames() const
{
    QStringList names = m_iterator.bindingNames();
    setError( m_iterator.lastError() );
    return names;
}


bool Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::isGraph() const
{
    bool b = m_iterator.isGraph();
    setError( m_iterator.lastError() );
    return b;
}


bool Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::isBinding() const
{
    bool b = m_iterator.isBinding();
    setError( m_iterator.lastError() );
    return b;
}


bool Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::isBool() const
{
    bool b = m_iterator.isBool();
    setError( m_iterator.lastError() );
    return b;
}


bool Soprano::Util::MultiCallProtectionQueryResultIteratorBackend::boolValue() const
{
    bool b = m_iterator.boolValue();
    setError( m_iterator.lastError() );
    return b;
}
