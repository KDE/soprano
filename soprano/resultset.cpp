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

#include "resultset.h"

#include "model.h"
#include "node.h"
#include "queryresult.h"

#include <QtCore/QSharedData>


namespace Soprano {

class ResultSet::Private : public QSharedData
{
public:
    Private()
        : queryResult( 0 ) {
    }

    ~Private() {
        delete queryResult;
    }

    QueryResult* queryResult;
};

ResultSet::ResultSet()
{
    d = new Private;
}

ResultSet::ResultSet( QueryResult *qr )
{
    d = new Private;
    d->queryResult = qr;
}

ResultSet::ResultSet( const ResultSet& other)
{
    d = other.d;
}

ResultSet::~ResultSet()
{
}

ResultSet& ResultSet::operator=( const ResultSet& other )
{
    d = other.d;
}

bool ResultSet::next() const
{
    return ( d->queryResult ? d->queryResult->next() : false );
}

Soprano::Node ResultSet::binding( const QString &name ) const
{
    return ( d->queryResult ? d->queryResult->binding( name ) : Node() );
}

Soprano::Node ResultSet::binding( int offset ) const
{
    return ( d->queryResult ? d->queryResult->binding( offset ) : Node() );
}

int ResultSet::bindingCount() const
{
    return ( d->queryResult ? d->queryResult->bindingCount() : 0 );
}

QStringList ResultSet::bindingNames() const
{
    return ( d->queryResult ? d->queryResult->bindingNames() : QStringList() );
}

bool ResultSet::isGraph() const
{
    return ( d->queryResult ? d->queryResult->isGraph() : false );
}

bool ResultSet::isBinding() const
{
    return ( d->queryResult ? d->queryResult->isBinding() : false );
}

bool ResultSet::isBool() const
{
    return ( d->queryResult ? d->queryResult->isBool() : false );
}

bool ResultSet::boolValue() const
{
    return ( d->queryResult ? d->queryResult->boolValue() : false );
}

Soprano::Model *ResultSet::model() const
{
    return ( d->queryResult ? d->queryResult->model() : 0 );
}

}
