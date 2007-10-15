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

#include "mutexstatementiteratorbackend.h"
#include "mutexmodel.h"

#include "statement.h"
#include "error.h"


Soprano::Util::MutexStatementIteratorBackend::MutexStatementIteratorBackend( const StatementIterator& it, MutexModel* model )
    : IteratorBackend<Statement>(),
      MutexIteratorBase( model ),
      m_iterator( it )
{
}


Soprano::Util::MutexStatementIteratorBackend::~MutexStatementIteratorBackend()
{
}


bool Soprano::Util::MutexStatementIteratorBackend::next()
{
    bool haveNext = m_iterator.next();
    setError( m_iterator.lastError() );
    return haveNext;
}


Soprano::Statement Soprano::Util::MutexStatementIteratorBackend::current() const
{
    Statement s = m_iterator.current();
    setError( m_iterator.lastError() );
    return s;
}


void Soprano::Util::MutexStatementIteratorBackend::close()
{
    m_iterator.close();
    setError( m_iterator.lastError() );
    remove();
}
