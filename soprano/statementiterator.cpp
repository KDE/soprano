/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "statementiterator.h"
#include "statementiteratorprivate.h"
#include "statement.h"


Soprano::StatementIterator::StatementIterator()
{
}

Soprano::StatementIterator::StatementIterator( StatementIteratorPrivate *sti )
    : d( sti )
{
}

Soprano::StatementIterator::StatementIterator( const StatementIterator &other )
    : d( other.d )
{
}

Soprano::StatementIterator::~StatementIterator()
{
}

Soprano::StatementIterator& Soprano::StatementIterator::operator=( const StatementIterator& other )
{
  d = other.d;
  return *this;
}

bool Soprano::StatementIterator::hasNext() const
{
  return isValid() ? d->hasNext() : false;
}

Soprano::Statement Soprano::StatementIterator::next() const
{
  return isValid() ? d->next() : Statement();
}

bool Soprano::StatementIterator::isValid() const
{
  return !isEmpty();
}

bool Soprano::StatementIterator::isEmpty() const
{
  return d == 0;
}
