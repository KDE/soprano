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
#include "statementiteratorbackend.h"
#include "statement.h"


// This is a dummy shared class that is never intended to be detached
// since we cannot actually copy the backend. So we only use the ref-count
// feature of QSharedData
class Soprano::StatementIterator::Private : public QSharedData
{
public:
    Private()
        : backend( 0 ) {
    }

    ~Private() {
        delete backend;
    }

    StatementIteratorBackend* backend;
};


Soprano::StatementIterator::StatementIterator()
    : d( new Private() )
{
}

Soprano::StatementIterator::StatementIterator( StatementIteratorBackend *sti )
    : d( new Private() )
{
    d->backend = sti;
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

bool Soprano::StatementIterator::next()
{
    // some evil hacking to avoid detachment of the shared data
    const Private* cd = d.constData();
    return isValid() ? cd->backend->next() : false;
}

Soprano::Statement Soprano::StatementIterator::current() const
{
    return isValid() ? d->backend->current() : Statement();
}

Soprano::Statement Soprano::StatementIterator::operator*() const
{
    return current();
}

bool Soprano::StatementIterator::isValid() const
{
  return !isEmpty();
}

bool Soprano::StatementIterator::isEmpty() const
{
  return d->backend == 0;
}
