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

#include "model.h"
#include "node.h"
#include "queryresult.h"

#include "resultset.h"

namespace Soprano {

ResultSet::ResultSet()
    : d(0)
{
}

ResultSet::ResultSet( QueryResult *qr ): d(qr)
{
}

ResultSet::~ResultSet()
{
    delete d;
}

bool ResultSet::next() const
{
    return ( d ? d->next() : false );
}

Soprano::Node ResultSet::binding( const QString &name ) const
{
    return ( d ? d->binding( name ) : Node() );
}

Soprano::Node ResultSet::binding( int offset ) const
{
    return ( d ? d->binding( offset ) : Node() );
}

int ResultSet::bindingCount() const
{
    return ( d ? d->bindingCount() : 0 );
}

QStringList ResultSet::bindingNames() const
{
    return ( d ? d->bindingNames() : QStringList() );
}

bool ResultSet::isGraph() const
{
    return ( d ? d->isGraph() : false );
}

bool ResultSet::isBinding() const
{
    return ( d ? d->isBinding() : false );
}

bool ResultSet::isBool() const
{
    return ( d ? d->isBool() : false );
}

bool ResultSet::boolValue() const
{
    return ( d ? d->boolValue() : false );
}

Soprano::Model *ResultSet::model() const
{
    return ( d ? d->model() : 0 );
}

}
