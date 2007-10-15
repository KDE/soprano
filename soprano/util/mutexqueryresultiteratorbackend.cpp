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

#include "mutexqueryresultiteratorbackend.h"
#include "mutexmodel.h"

#include "error.h"
#include "statement.h"


Soprano::Util::MutexQueryResultIteratorBackend::MutexQueryResultIteratorBackend( const QueryResultIterator& it, MutexModel* model )
    : QueryResultIteratorBackend(),
      MutexIteratorBase( model ),
      m_iterator( it )
{
}


Soprano::Util::MutexQueryResultIteratorBackend::~MutexQueryResultIteratorBackend()
{
}


bool Soprano::Util::MutexQueryResultIteratorBackend::next()
{
    bool haveNext = m_iterator.next();
    setError( m_iterator.lastError() );
    return haveNext;
}


Soprano::BindingSet Soprano::Util::MutexQueryResultIteratorBackend::current() const
{
    BindingSet s = m_iterator.current();
    setError( m_iterator.lastError() );
    return s;
}


void Soprano::Util::MutexQueryResultIteratorBackend::close()
{
    m_iterator.close();
    setError( m_iterator.lastError() );
    remove();
}


Soprano::Statement Soprano::Util::MutexQueryResultIteratorBackend::currentStatement() const
{
    Statement s = m_iterator.currentStatement();
    setError( m_iterator.lastError() );
    return s;
}


Soprano::Node Soprano::Util::MutexQueryResultIteratorBackend::binding( const QString &name ) const
{
    Node n = m_iterator.binding( name );
    setError( m_iterator.lastError() );
    return n;
}


Soprano::Node Soprano::Util::MutexQueryResultIteratorBackend::binding( int offset ) const
{
    Node n = m_iterator.binding( offset );
    setError( m_iterator.lastError() );
    return n;
}


int Soprano::Util::MutexQueryResultIteratorBackend::bindingCount() const
{
    int cnt = m_iterator.bindingCount();
    setError( m_iterator.lastError() );
    return cnt;
}


QStringList Soprano::Util::MutexQueryResultIteratorBackend::bindingNames() const
{
    QStringList names = m_iterator.bindingNames();
    setError( m_iterator.lastError() );
    return names;
}


bool Soprano::Util::MutexQueryResultIteratorBackend::isGraph() const
{
    bool b = m_iterator.isGraph();
    setError( m_iterator.lastError() );
    return b;
}


bool Soprano::Util::MutexQueryResultIteratorBackend::isBinding() const
{
    bool b = m_iterator.isBinding();
    setError( m_iterator.lastError() );
    return b;
}


bool Soprano::Util::MutexQueryResultIteratorBackend::isBool() const
{
    bool b = m_iterator.isBool();
    setError( m_iterator.lastError() );
    return b;
}


bool Soprano::Util::MutexQueryResultIteratorBackend::boolValue() const
{
    bool b = m_iterator.boolValue();
    setError( m_iterator.lastError() );
    return b;
}
