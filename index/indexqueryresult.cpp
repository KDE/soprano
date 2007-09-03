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

#include "indexqueryresult.h"
#include "node.h"

class Soprano::Index::QueryResult::Private : public QSharedData
{
    public:
    Private( const Node& res = Node(), double s = 0.0 )
        : resource( res ),
          score( s ) {
    }

    Node resource;
    double score;
};


Soprano::Index::QueryResult::QueryResult()
    : d( new Private() )
{
}


Soprano::Index::QueryResult::QueryResult( const Node& res,  double s )
    : d( new Private( res, s ) )
{
}


Soprano::Index::QueryResult::QueryResult( const QueryResult& other )
{
    d = other.d;
}


Soprano::Index::QueryResult::~QueryResult()
{
}


Soprano::Index::QueryResult& Soprano::Index::QueryResult::operator=( const QueryResult& other )
{
    d = other.d;
    return *this;
}


double Soprano::Index::QueryResult::score() const
{
    return d->score;
}


Soprano::Node Soprano::Index::QueryResult::resource() const
{
    return d->resource;
}
