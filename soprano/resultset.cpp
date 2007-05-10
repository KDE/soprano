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

#include "resultset.h"

#include "model.h"
#include "node.h"
#include "statement.h"
#include "queryresult.h"

#include <QtCore/QSharedData>


class Soprano::ResultSet::Private : public QSharedData
{
public:
    Private( QueryResult* qr = 0 )
        : queryResult( qr ) {
    }

    ~Private() {
        delete queryResult;
    }

    QueryResult* queryResult;
};


Soprano::ResultSet::ResultSet()
{
    d = new Private;
}


Soprano::ResultSet::ResultSet( QueryResult *qr )
{
    d = new Private( qr );
}


Soprano::ResultSet::ResultSet( const ResultSet& other )
{
    d = other.d;
}


Soprano::ResultSet::~ResultSet()
{
}


Soprano::ResultSet& Soprano::ResultSet::operator=( const ResultSet& other )
{
    d = other.d;
    return *this;
}


bool Soprano::ResultSet::next()
{
    // make sure the data is not detached
    const ResultSet* that = const_cast<const ResultSet*>( this );
    return ( that->d->queryResult ? that->d->queryResult->next() : false );
}


Soprano::Statement Soprano::ResultSet::currentStatement() const
{
    return ( d->queryResult ? d->queryResult->currentStatement() : Statement() );
}


Soprano::Node Soprano::ResultSet::binding( const QString &name ) const
{
    return ( d->queryResult ? d->queryResult->binding( name ) : Node() );
}


Soprano::Node Soprano::ResultSet::binding( int offset ) const
{
    return ( d->queryResult ? d->queryResult->binding( offset ) : Node() );
}


int Soprano::ResultSet::bindingCount() const
{
    return ( d->queryResult ? d->queryResult->bindingCount() : 0 );
}


QStringList Soprano::ResultSet::bindingNames() const
{
    return ( d->queryResult ? d->queryResult->bindingNames() : QStringList() );
}


bool Soprano::ResultSet::isGraph() const
{
    return ( d->queryResult ? d->queryResult->isGraph() : false );
}


bool Soprano::ResultSet::isBinding() const
{
    return ( d->queryResult ? d->queryResult->isBinding() : false );
}


bool Soprano::ResultSet::isBool() const
{
    return ( d->queryResult ? d->queryResult->isBool() : false );
}


bool Soprano::ResultSet::boolValue() const
{
    return ( d->queryResult ? d->queryResult->boolValue() : false );
}


Soprano::Model *Soprano::ResultSet::model() const
{
    return ( d->queryResult ? d->queryResult->model() : 0 );
}
