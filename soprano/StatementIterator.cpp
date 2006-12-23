/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "Statement.h"
#include "StatementIteratorPrivate.h"
#include "StatementIterator.h"

using namespace Soprano;

StatementIterator::StatementIterator() : d( 0L )
{
}

StatementIterator::StatementIterator( StatementIteratorPrivate *sti ) : d( sti )
{
}

StatementIterator::StatementIterator( const StatementIterator &other ) : d( other.d )
{
}

StatementIterator::~StatementIterator()
{
}

StatementIterator& StatementIterator::operator=( const StatementIterator& other )
{
  d = other.d;
  return *this;
}

bool StatementIterator::hasNext() const
{
  return isValid() ? d->hasNext() : false;
}

const Soprano::Statement StatementIterator::next() const
{
  return isValid() ? d->next() : Statement();
}

bool StatementIterator::isValid() const
{
  return !isEmpty();
}

bool StatementIterator::isEmpty() const
{
  return d == 0L;
}
