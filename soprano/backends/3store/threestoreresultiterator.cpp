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

#include "threestoreresultiterator.h"

#include "statement.h"


Soprano::ThreeStore::ResultIterator::ResultIterator( const QueryResultIterator& r )
    : Soprano::StatementIteratorBackend(),
      m_result( r )
{
}


Soprano::ThreeStore::ResultIterator::~ResultIterator()
{
}


bool Soprano::ThreeStore::ResultIterator::next()
{
    return m_result.next();
}


Soprano::Statement Soprano::ThreeStore::ResultIterator::current() const
{
    return m_result.currentStatement();
}
