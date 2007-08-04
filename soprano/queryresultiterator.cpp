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

#include "queryresultiterator.h"
#include "queryresultiteratorbackend.h"
#include "model.h"
#include "node.h"
#include "statement.h"

#include <QtCore/QSharedData>


class Soprano::QueryResultIterator::Private : public QSharedData
{
public:
    Private( QueryResultIteratorBackend* qr = 0 )
        : queryResult( qr ) {
    }

    ~Private() {
        delete queryResult;
    }

    QueryResultIteratorBackend* queryResult;
};


Soprano::QueryResultIterator::QueryResultIterator()
{
    d = new Private;
}


Soprano::QueryResultIterator::QueryResultIterator( QueryResultIteratorBackend *qr )
{
    d = new Private( qr );
}


Soprano::QueryResultIterator::QueryResultIterator( const QueryResultIterator& other )
{
    d = other.d;
}


Soprano::QueryResultIterator::~QueryResultIterator()
{
}


Soprano::QueryResultIterator& Soprano::QueryResultIterator::operator=( const QueryResultIterator& other )
{
    d = other.d;
    return *this;
}


bool Soprano::QueryResultIterator::next()
{
    // some evil hacking to avoid detachment of the shared data
    const Private* cd = d.constData();
    return ( cd->queryResult ? cd->queryResult->next() : false );
}


Soprano::Statement Soprano::QueryResultIterator::currentStatement() const
{
    return ( d->queryResult ? d->queryResult->currentStatement() : Statement() );
}


Soprano::Node Soprano::QueryResultIterator::binding( const QString &name ) const
{
    return ( d->queryResult ? d->queryResult->binding( name ) : Node() );
}


Soprano::Node Soprano::QueryResultIterator::binding( int offset ) const
{
    return ( d->queryResult ? d->queryResult->binding( offset ) : Node() );
}


int Soprano::QueryResultIterator::bindingCount() const
{
    return ( d->queryResult ? d->queryResult->bindingCount() : 0 );
}


QStringList Soprano::QueryResultIterator::bindingNames() const
{
    return ( d->queryResult ? d->queryResult->bindingNames() : QStringList() );
}


bool Soprano::QueryResultIterator::isGraph() const
{
    return ( d->queryResult ? d->queryResult->isGraph() : false );
}


bool Soprano::QueryResultIterator::isBinding() const
{
    return ( d->queryResult ? d->queryResult->isBinding() : false );
}


bool Soprano::QueryResultIterator::isBool() const
{
    return ( d->queryResult ? d->queryResult->isBool() : false );
}


bool Soprano::QueryResultIterator::boolValue() const
{
    return ( d->queryResult ? d->queryResult->boolValue() : false );
}


bool Soprano::QueryResultIterator::isValid() const
{
    return !isEmpty();
}

bool Soprano::QueryResultIterator::isEmpty() const
{
    return d->queryResult == 0;
}
